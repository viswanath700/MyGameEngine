// Header Files
//=============
#include "../../Graphics/Graphics.h"
#include "../DebugShapes.h"
//#include "D3D9Types.h"

// Interface
//==========

void eae6320::Graphics::DebugLine::LoadDebugLine()
{
	// create the vertices using the CUSTOMVERTEX struct
	CUSTOMVERTEX vertices[] =
	{
		{ m_startPoint.x, m_startPoint.y, m_startPoint.z, (uint8_t)(m_color.x * 255), (uint8_t)(m_color.y * 255), (uint8_t)(m_color.z * 255), },
		{ m_endPoint.x, m_endPoint.y, m_endPoint.z, (uint8_t)(m_color.x * 255), (uint8_t)(m_color.y * 255), (uint8_t)(m_color.z * 255), },
		// Using same color for the both vertices to draw line of same color
	};

	// create a vertex buffer interface called i_buffer
	const unsigned int vertexCount = 2;
	const unsigned int bufferSize = vertexCount * sizeof(CUSTOMVERTEX);
	DWORD usage = 0;
	// We will define our own vertex format
	const DWORD useSeparateVertexDeclaration = 0;
	// Place the vertex buffer into memory that Direct3D thinks is the most appropriate
	const D3DPOOL useDefaultPool = D3DPOOL_DEFAULT;
	HANDLE* const notUsed = NULL;
	Graphics::GetLocalDirect3dDevice()->CreateVertexBuffer(bufferSize, usage, useSeparateVertexDeclaration, useDefaultPool,
		&m_vBuffer, notUsed);
	
	VOID* pVoid;    // a void pointer

	// lock v_buffer and load the vertices into it
	m_vBuffer->Lock(0, 0, (void**)&pVoid, 0);
	memcpy(pVoid, vertices, sizeof(vertices));
	m_vBuffer->Unlock();

	return;
}

void eae6320::Graphics::DebugLine::DrawLine()
{
	Graphics::GetLocalDirect3dDevice()->SetStreamSource(0, m_vBuffer, 0, sizeof(CUSTOMVERTEX));

	const unsigned int primitiveCount = 1;
	Graphics::GetLocalDirect3dDevice()->DrawPrimitive(D3DPT_LINELIST, 0, primitiveCount);
}

void eae6320::Graphics::DebugBox::LoadDebugBox()
{
	// create the vertices using the CUSTOMVERTEX struct
	CUSTOMVERTEX vertices[] =
	{
		{ -3.0f, 3.0f, -3.0f, 0, 0, 255, },
		{ 3.0f, 3.0f, -3.0f, 0, 255, 0, },
		{ -3.0f, -3.0f, -3.0f, 255, 0, 0, },
		{ 3.0f, -3.0f, -3.0f, 0, 255, 255, },
		{ -3.0f, 3.0f, 3.0f, 0, 0, 255, },
		{ 3.0f, 3.0f, 3.0f, 255, 0, 0, },
		{ -3.0f, -3.0f, 3.0f, 0, 255, 0, },
		{ 3.0f, -3.0f, 3.0f, 0, 255, 255, },
	};

	// create a vertex buffer interface called v_buffer
	const unsigned int vertexCount = 8;
	const unsigned int bufferSize = vertexCount * sizeof(CUSTOMVERTEX);
	DWORD usage = 0;
	// We will define our own vertex format
	const DWORD useSeparateVertexDeclaration = 0;
	// Place the vertex buffer into memory that Direct3D thinks is the most appropriate
	const D3DPOOL useDefaultPool = D3DPOOL_DEFAULT;
	HANDLE* const notUsed = NULL;
	Graphics::GetLocalDirect3dDevice()->CreateVertexBuffer(bufferSize, usage, useSeparateVertexDeclaration, useDefaultPool,
		&m_vBuffer, notUsed);

	VOID* pVoid;    // a void pointer

	// lock v_buffer and load the vertices into it
	m_vBuffer->Lock(0, 0, (void**)&pVoid, 0);
	memcpy(pVoid, vertices, sizeof(vertices));
	m_vBuffer->Unlock();

	// create the indices using an int array
	short indices[] =
	{
		0, 1, 2,    // side 1
		2, 1, 3,
		4, 0, 6,    // side 2
		6, 0, 2,
		7, 5, 6,    // side 3
		6, 5, 4,
		3, 1, 7,    // side 4
		7, 1, 5,
		4, 5, 0,    // side 5
		0, 5, 1,
		3, 7, 2,    // side 6
		2, 7, 6,
	};

	// create an index buffer interface called i_buffer
	Graphics::GetLocalDirect3dDevice()->CreateIndexBuffer(36 * sizeof(short),
		0,
		D3DFMT_INDEX16,
		D3DPOOL_MANAGED,
		&m_iBuffer,
		NULL);

	// lock i_buffer and load the indices into it
	m_iBuffer->Lock(0, 0, (void**)&pVoid, 0);
	memcpy(pVoid, indices, sizeof(indices));
	m_iBuffer->Unlock();
}

void eae6320::Graphics::DebugBox::DrawBox()
{
	// select the vertex and index buffers to use
	Graphics::GetLocalDirect3dDevice()->SetStreamSource(0, m_vBuffer, 0, sizeof(CUSTOMVERTEX));
	Graphics::GetLocalDirect3dDevice()->SetIndices(m_iBuffer);

	// draw the cube
	Graphics::GetLocalDirect3dDevice()->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 8, 0, 12);
}

void eae6320::Graphics::DebugSphere::DrawSphere()
{

}