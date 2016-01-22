// Header Files
//=============

#include "Mesh.h"

#include <cassert>
#include <sstream>
#include <fstream>
#include "../UserOutput/UserOutput.h"

// Interface
//==========

bool eae6320::Graphics::Mesh::LoadMesh()
{
	if (!ReadFromBinaryMeshFile(m_meshPath)) {
		eae6320::UserOutput::Print("There was a problem reading binary file " + std::string(m_meshPath));
		return false;
	}

	if (!LoadVertexAndIndexData())
	{
		return false;
	}

	// Deleting member vertex and index data as they have been loaded
	{
		delete[] m_binaryReadBuffer;
		m_binaryReadBuffer = NULL;

		m_vertexData = NULL;
		m_indexData = NULL;
	}

	return true;
}

bool eae6320::Graphics::Mesh::ReadFromBinaryMeshFile(const char* const i_path) {

	std::ifstream inputBinaryMeshFile(i_path, std::ifstream::binary);

	if (inputBinaryMeshFile) {

		// Get length of the file
		inputBinaryMeshFile.seekg(0, inputBinaryMeshFile.end);
		size_t fileLength = (size_t)inputBinaryMeshFile.tellg();
		inputBinaryMeshFile.seekg(0, inputBinaryMeshFile.beg);

		// Read and store into buffer
		m_binaryReadBuffer = new char[fileLength];
		inputBinaryMeshFile.read(m_binaryReadBuffer, fileLength);

		// Extracting corresponding info after reading binary
		m_vertexCount = *reinterpret_cast<uint32_t*>(m_binaryReadBuffer);
		m_vertexData = reinterpret_cast<sVertex*>(m_binaryReadBuffer + 4 * 2);

		m_indexCount = *reinterpret_cast<uint32_t*>(m_binaryReadBuffer + 4);
		m_indexData = reinterpret_cast<uint32_t*>(m_binaryReadBuffer + 4 * 2 + sizeof(sVertex) * m_vertexCount);

		return true;
	}

	return false;
}