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
	
}

void eae6320::Graphics::DebugBox::DrawBox()
{
	
}

void eae6320::Graphics::DebugSphere::DrawSphere()
{

}