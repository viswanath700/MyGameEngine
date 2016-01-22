// Header Files
//=============

#include "Effect.h"

#include <cassert>
#include <sstream>
#include <fstream>
#include "../UserOutput/UserOutput.h"

// Interface
//==========

bool eae6320::Graphics::Effect::ReadFromBinaryEffectFile()
{
	std::ifstream inputBinaryEffectFile(m_path_effect, std::ifstream::binary);

	if (inputBinaryEffectFile) {

		// Get length of the file
		inputBinaryEffectFile.seekg(0, inputBinaryEffectFile.end);
		size_t fileLength = (size_t)inputBinaryEffectFile.tellg();
		inputBinaryEffectFile.seekg(0, inputBinaryEffectFile.beg);

		// Read and store into buffer
		m_binaryReadBuffer = new char[fileLength];
		inputBinaryEffectFile.read(m_binaryReadBuffer, fileLength);

		// Extracting corresponding info after reading binary
		if (inputBinaryEffectFile) {
			m_path_vertexShader = m_binaryReadBuffer;
			m_path_fragmentShader = m_binaryReadBuffer + std::strlen(m_path_vertexShader) + 1;
			m_renderStates = reinterpret_cast<uint8_t*>(m_binaryReadBuffer + std::strlen(m_path_vertexShader) + 1 + std::strlen(m_path_fragmentShader) + 1);
		}

		return true;
	}

	return false;
}