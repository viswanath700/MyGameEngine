#include "AudioControl.h"

#include <xaudio2.h>
#include <assert.h>

#include <vector>

#include "../../Engine/Graphics/Graphics.h"
#include "../../Engine/UserOutput/UserOutput.h"

#define fourccRIFF 'FFIR'
#define fourccDATA 'atad'
#define fourccFMT ' tmf'
#define fourccWAVE 'EVAW'
#define fourccXWMA 'AMWX'
#define fourccDPDS 'sdpd'

namespace eae6320
{
	// Namespace Variables
	namespace Audio
	{
		IXAudio2* s_pXAudio2 = NULL;
		IXAudio2MasteringVoice* s_pMasterVoice = NULL;
		std::vector<IXAudio2SourceVoice*> s_SourceVoices;
		std::vector<XAUDIO2_BUFFER> s_AudioBuffers;

		int s_CurrentPlayingAudio;
	}

	HRESULT FindChunk(HANDLE hFile, DWORD fourcc, DWORD & dwChunkSize, DWORD & dwChunkDataPosition)
	{
		HRESULT hr = S_OK;
		if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, 0, NULL, FILE_BEGIN))
			return HRESULT_FROM_WIN32(GetLastError());

		DWORD dwChunkType;
		DWORD dwChunkDataSize;
		DWORD dwRIFFDataSize = 0;
		DWORD dwFileType;
		DWORD bytesRead = 0;
		DWORD dwOffset = 0;

		while (hr == S_OK)
		{
			DWORD dwRead;
			if (0 == ReadFile(hFile, &dwChunkType, sizeof(DWORD), &dwRead, NULL))
				hr = HRESULT_FROM_WIN32(GetLastError());

			if (0 == ReadFile(hFile, &dwChunkDataSize, sizeof(DWORD), &dwRead, NULL))
				hr = HRESULT_FROM_WIN32(GetLastError());

			switch (dwChunkType)
			{
			case fourccRIFF:
				dwRIFFDataSize = dwChunkDataSize;
				dwChunkDataSize = 4;
				if (0 == ReadFile(hFile, &dwFileType, sizeof(DWORD), &dwRead, NULL))
					hr = HRESULT_FROM_WIN32(GetLastError());
				break;

			default:
				if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, dwChunkDataSize, NULL, FILE_CURRENT))
					return HRESULT_FROM_WIN32(GetLastError());
			}

			dwOffset += sizeof(DWORD) * 2;

			if (dwChunkType == fourcc)
			{
				dwChunkSize = dwChunkDataSize;
				dwChunkDataPosition = dwOffset;
				return S_OK;
			}

			dwOffset += dwChunkDataSize;

			if (bytesRead >= dwRIFFDataSize) return S_FALSE;

		}

		return S_OK;

	}

	HRESULT ReadChunkData(HANDLE hFile, void * buffer, DWORD buffersize, DWORD bufferoffset)
	{
		HRESULT hr = S_OK;
		if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, bufferoffset, NULL, FILE_BEGIN))
			return HRESULT_FROM_WIN32(GetLastError());
		DWORD dwRead;
		if (0 == ReadFile(hFile, buffer, buffersize, &dwRead, NULL))
			hr = HRESULT_FROM_WIN32(GetLastError());
		return hr;
	}

	void Audio::PlayAudio(int i_Index)
	{
		s_CurrentPlayingAudio = i_Index;

		s_SourceVoices[i_Index]->FlushSourceBuffers();
		s_SourceVoices[i_Index]->SubmitSourceBuffer(&s_AudioBuffers[i_Index]);
		HRESULT hr;
		hr = s_SourceVoices[i_Index]->Start(0);
		assert(SUCCEEDED(hr));
	}

	void Audio::StopAudio(int i_Index)
	{
		HRESULT hr;
		hr = s_SourceVoices[i_Index]->Stop();
		assert(SUCCEEDED(hr));
	}

	void Audio::StopAll()
	{
		s_SourceVoices[s_CurrentPlayingAudio]->Stop();
	}

	void Audio::SetVolume(int i_Index, float currentDistance, float maxDistance, float minDistance)
	{
		if (currentDistance > maxDistance)
			s_SourceVoices[i_Index]->SetVolume(0.0f);
		/*else if (currentDistance < maxDistance)
			s_SourceVoices[i_Index]->SetVolume(1.0f);*/
		else
		{
			s_SourceVoices[i_Index]->SetVolume((maxDistance - currentDistance) / maxDistance);
		}
	}

	bool Audio::AddAudioFile(const char* i_AudioPath, bool bLoop, float i_InitialVolume)
	{
		WAVEFORMATEXTENSIBLE wfx = { 0 };
		XAUDIO2_BUFFER buffer = { 0 };

		// Open the file
		HANDLE hFile = CreateFile(
			i_AudioPath,
			GENERIC_READ,
			FILE_SHARE_READ,
			NULL,
			OPEN_EXISTING,
			0,
			NULL);

		if (INVALID_HANDLE_VALUE == hFile)
			eae6320::UserOutput::Print("Audio File not found");

		SetFilePointer(hFile, 0, NULL, FILE_BEGIN);

		DWORD dwChunkSize;
		DWORD dwChunkPosition;
		//check the file type, should be fourccWAVE or 'XWMA'
		FindChunk(hFile, fourccRIFF, dwChunkSize, dwChunkPosition);
		DWORD filetype;
		ReadChunkData(hFile, &filetype, sizeof(DWORD), dwChunkPosition);
		if (filetype != fourccWAVE)
			return false;

		FindChunk(hFile, fourccFMT, dwChunkSize, dwChunkPosition);
		ReadChunkData(hFile, &wfx, dwChunkSize, dwChunkPosition);

		//fill out the audio data buffer with the contents of the fourccDATA chunk
		FindChunk(hFile, fourccDATA, dwChunkSize, dwChunkPosition);
		BYTE * pDataBuffer = new BYTE[dwChunkSize];
		ReadChunkData(hFile, pDataBuffer, dwChunkSize, dwChunkPosition);

		buffer.AudioBytes = dwChunkSize;  //buffer containing audio data
		buffer.pAudioData = pDataBuffer;  //size of the audio buffer in bytes
		buffer.Flags = XAUDIO2_END_OF_STREAM; // tell the source voice not to expect any data after this buffer
		if (bLoop)
		{
			buffer.LoopLength = 0;
			buffer.LoopCount = XAUDIO2_LOOP_INFINITE;
		}

		HRESULT hr;
		IXAudio2SourceVoice* pSourceVoice;
		if (FAILED(hr = s_pXAudio2->CreateSourceVoice(&pSourceVoice, (WAVEFORMATEX*)&wfx)))
			return false;

		if (FAILED(hr = pSourceVoice->SubmitSourceBuffer(&buffer)))
			return false;

		pSourceVoice->SetVolume(i_InitialVolume);

		s_SourceVoices.push_back(pSourceVoice);
		s_AudioBuffers.push_back(buffer);
	}

	void Audio::Initialize()
	{
		HRESULT hr = XAudio2Create(&s_pXAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR);
		assert(SUCCEEDED(hr));

		hr = s_pXAudio2->CreateMasteringVoice(&s_pMasterVoice);
		assert(SUCCEEDED(hr));
	}

	void Audio::SetVolumeAll(int count)
	{
		float volume = (float)(Graphics::s_debugMenuSlider->sliderCurrentPosition) / 20.0f;

		if (volume < 0)
			volume = 0;

		if (volume > 1)
			volume = 1;

		for (int i = 0; i < count; i++)
			s_SourceVoices[i]->SetVolume(volume);
	}
}