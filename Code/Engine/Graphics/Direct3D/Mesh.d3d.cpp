// Header Files
//=============
#include "../Mesh.h"
#include "../Graphics.h"

#include <cassert>
#include "../../UserOutput/UserOutput.h"

// Interface
//==========

eae6320::Graphics::Mesh::Mesh(char* i_meshPath)
{
	m_vertexBuffer = NULL;
	m_indexBuffer = NULL;
	m_vertexDeclaration = NULL;

	m_meshPath = i_meshPath;
}

void eae6320::Graphics::Mesh::DrawMesh()
{
	HRESULT result;
	// Bind a specific vertex buffer to the device as a data source
	{
		// There can be multiple streams of data feeding the display adaptor at the same time
		const unsigned int streamIndex = 0;
		// It's possible to start streaming data in the middle of a vertex buffer
		const unsigned int bufferOffset = 0;
		// The "stride" defines how large a single vertex is in the stream of data
		const unsigned int bufferStride = sizeof(sVertex);
		result = GetLocalDirect3dDevice()->SetStreamSource(streamIndex, m_vertexBuffer, bufferOffset, bufferStride);
		assert(SUCCEEDED(result));
	}
	// Bind a specific index buffer to the device as a data source
	{
		result = GetLocalDirect3dDevice()->SetIndices(m_indexBuffer);
		assert(SUCCEEDED(result));
	}
	// Render objects from the current streams
	{
		// We are using triangles as the "primitive" type,
		// and we have defined the vertex buffer as a triangle list
		// (meaning that every triangle is defined by three vertices)
		const D3DPRIMITIVETYPE primitiveType = D3DPT_TRIANGLELIST;
		// It's possible to start rendering primitives in the middle of the stream
		const unsigned int indexOfFirstVertexToRender = 0;
		const unsigned int indexOfFirstIndexToUse = 0;
		// We are drawing a square
		const unsigned int vertexCountToRender = m_vertexCount;
		const unsigned int vertexCountPerTriangle = 3;
		const unsigned int primitiveCountToRender = m_indexCount / vertexCountPerTriangle;
		result = GetLocalDirect3dDevice()->DrawIndexedPrimitive(primitiveType,
			indexOfFirstVertexToRender, indexOfFirstVertexToRender, vertexCountToRender,
			indexOfFirstIndexToUse, primitiveCountToRender);
		assert(SUCCEEDED(result));
	}
}

void eae6320::Graphics::Mesh::ReleaseMesh()
{
	if (m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = NULL;
	}
	if (m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = NULL;
	}
	if (m_vertexDeclaration)
	{
		Graphics::GetLocalDirect3dDevice()->SetVertexDeclaration(NULL);
		m_vertexDeclaration->Release();
		m_vertexDeclaration = NULL;
	}
}

bool eae6320::Graphics::Mesh::LoadVertexAndIndexData()
{
	if (!CreateVertexBuffer())
	{
		return false;
	}
	if (!CreateIndexBuffer())
	{
		return false;
	}

	return true;
}

bool eae6320::Graphics::Mesh::CreateIndexBuffer()
{
	// The usage tells Direct3D how this vertex buffer will be used
	DWORD usage = 0;
	{
		// The type of vertex processing should match what was specified when the device interface was created with CreateDevice()
		const HRESULT result = GetVertexProcessingUsage(usage);
		if (FAILED(result))
		{
			return false;
		}
		// Our code will only ever write to the buffer
		usage |= D3DUSAGE_WRITEONLY;
	}

	// Create an index buffer
	unsigned int bufferSize;
	{
		const unsigned int vertexCountPerTriangle = 3;
		const unsigned int triangleCount = m_indexCount / vertexCountPerTriangle;
		bufferSize = triangleCount * vertexCountPerTriangle * sizeof(uint32_t);
		// We'll use 32-bit indices in this class to keep things simple
		// (i.e. every index will be a 32 bit unsigned integer)
		const D3DFORMAT format = D3DFMT_INDEX32;
		// Place the index buffer into memory that Direct3D thinks is the most appropriate
		const D3DPOOL useDefaultPool = D3DPOOL_DEFAULT;
		HANDLE* notUsed = NULL;
		const HRESULT result = GetLocalDirect3dDevice()->CreateIndexBuffer(bufferSize, usage, format, useDefaultPool,
			&m_indexBuffer, notUsed);
		if (FAILED(result))
		{
			eae6320::UserOutput::Print("Direct3D failed to create an index buffer");
			return false;
		}
	}
	// Fill the index buffer with the triangles' connectivity data
	{
		// Before the index buffer can be changed it must be "locked"
		uint32_t* indexData;
		{
			const unsigned int lockEntireBuffer = 0;
			const DWORD useDefaultLockingBehavior = 0;
			const HRESULT result = m_indexBuffer->Lock(lockEntireBuffer, lockEntireBuffer,
				reinterpret_cast<void**>(&indexData), useDefaultLockingBehavior);
			if (FAILED(result))
			{
				eae6320::UserOutput::Print("Direct3D failed to lock the index buffer");
				return false;
			}
		}
		// Fill the buffer
		{
			for (unsigned int i = 0; i < m_indexCount; i ++)
				indexData[i] = m_indexData[i];
		}
		// The buffer must be "unlocked" before it can be used
		{
			const HRESULT result = m_indexBuffer->Unlock();
			if (FAILED(result))
			{
				eae6320::UserOutput::Print("Direct3D failed to unlock the index buffer");
				return false;
			}
		}
	}

	return true;
}

bool eae6320::Graphics::Mesh::CreateVertexBuffer()
{
	// The usage tells Direct3D how this vertex buffer will be used
	DWORD usage = 0;
	{
		// The type of vertex processing should match what was specified when the device interface was created with CreateDevice()
		const HRESULT result = GetVertexProcessingUsage(usage);
		if (FAILED(result))
		{
			return false;
		}
		// Our code will only ever write to the buffer
		usage |= D3DUSAGE_WRITEONLY;
	}

	// Initialize the vertex format
	{
		// These elements must match the VertexFormat::sVertex layout struct exactly.
		// They instruct Direct3D how to match the binary data in the vertex buffer
		// to the input elements in a vertex shader
		// (by using D3DDECLUSAGE enums here and semantics in the shader,
		// so that, for example, D3DDECLUSAGE_POSITION here matches with POSITION in shader code).
		// Note that OpenGL uses arbitrarily assignable number IDs to do the same thing.
		D3DVERTEXELEMENT9 vertexElements[] =
		{
			// Stream 0

			// POSITION
			// 3 floats == 12 bytes
			// Offset = 0
			{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },

			// NORMAL
			// 3 floats == 12 bytes
			// Offset = 12
			{ 0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },

			// COLOR0
			// D3DCOLOR == 4 bytes
			// Offset = 24
			{ 0, 24, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },

			//UV
			//2 floats = 8 bytes  
			//offset = 28
			{ 0, 28, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },

			// The following marker signals the end of the vertex declaration
			D3DDECL_END()
		};
		HRESULT result = GetLocalDirect3dDevice()->CreateVertexDeclaration(vertexElements, &m_vertexDeclaration);
		if (SUCCEEDED(result))
		{
			result = GetLocalDirect3dDevice()->SetVertexDeclaration(m_vertexDeclaration);
			if (FAILED(result))
			{
				eae6320::UserOutput::Print("Direct3D failed to set the vertex declaration");
				return false;
			}
		}
		else
		{
			eae6320::UserOutput::Print("Direct3D failed to create a Direct3D9 vertex declaration");
			return false;
		}
	}

	// Create a vertex buffer
	{
		const unsigned int vertexCount = m_vertexCount;
		const unsigned int bufferSize = vertexCount * sizeof(eae6320::Graphics::Mesh::sVertex);
		// We will define our own vertex format
		const DWORD useSeparateVertexDeclaration = 0;
		// Place the vertex buffer into memory that Direct3D thinks is the most appropriate
		const D3DPOOL useDefaultPool = D3DPOOL_DEFAULT;
		HANDLE* const notUsed = NULL;
		const HRESULT result = GetLocalDirect3dDevice()->CreateVertexBuffer(bufferSize, usage, useSeparateVertexDeclaration, useDefaultPool,
			&m_vertexBuffer, notUsed);
		if (FAILED(result))
		{
			eae6320::UserOutput::Print("Direct3D failed to create a vertex buffer");
			return false;
		}
	}
	// Fill the vertex buffer with the triangle's vertices
	{
		// Before the vertex buffer can be changed it must be "locked"
		eae6320::Graphics::Mesh::sVertex* vertexData;
		{
			const unsigned int lockEntireBuffer = 0;
			const DWORD useDefaultLockingBehavior = 0;
			const HRESULT result = m_vertexBuffer->Lock(lockEntireBuffer, lockEntireBuffer,
				reinterpret_cast<void**>(&vertexData), useDefaultLockingBehavior);
			if (FAILED(result))
			{
				eae6320::UserOutput::Print("Direct3D failed to lock the vertex buffer");
				return false;
			}
		}
		// Fill the buffer
		{
			for (unsigned int i = 0; i < m_vertexCount; ++i)
				vertexData[i] = m_vertexData[i];
		}
		// The buffer must be "unlocked" before it can be used
		{
			const HRESULT result = m_vertexBuffer->Unlock();
			if (FAILED(result))
			{
				eae6320::UserOutput::Print("Direct3D failed to unlock the vertex buffer");
				return false;
			}
		}
	}

	return true;
}

HRESULT eae6320::Graphics::Mesh::GetVertexProcessingUsage(DWORD& o_usage)
{
	D3DDEVICE_CREATION_PARAMETERS deviceCreationParameters;
	const HRESULT result = GetLocalDirect3dDevice()->GetCreationParameters(&deviceCreationParameters);
	if (SUCCEEDED(result))
	{
		DWORD vertexProcessingType = deviceCreationParameters.BehaviorFlags &
			(D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_MIXED_VERTEXPROCESSING | D3DCREATE_SOFTWARE_VERTEXPROCESSING);
		o_usage = (vertexProcessingType != D3DCREATE_SOFTWARE_VERTEXPROCESSING) ? 0 : D3DUSAGE_SOFTWAREPROCESSING;
	}
	else
	{
		eae6320::UserOutput::Print("Direct3D failed to get the device's creation parameters");
	}
	return result;
}
