/*
This file contains the function declarations for Mesh
*/

#ifndef EAE6320_MESH_H
#define EAE6320_MESH_H

// Header Files
//=============

#include <cstdint>

#ifdef EAE6320_PLATFORM_D3D
#include <d3d9.h>
#endif // EAE6320_PLATFORM_D3D

#ifdef EAE6320_PLATFORM_GL
#include "../../External/OpenGlExtensions/OpenGlExtensions.h"
#endif // EAE6320_PLATFORM_GL

// Interface
//==========

namespace eae6320
{
	namespace Graphics
	{
		class Mesh {

#ifdef EAE6320_PLATFORM_D3D
			IDirect3DVertexBuffer9* m_vertexBuffer;
			IDirect3DIndexBuffer9* m_indexBuffer;
			IDirect3DVertexDeclaration9* m_vertexDeclaration;

		public:
			struct sVertex
			{
				// POSITION
				// 3 floats == 12 bytes
				// Offset = 0
				float x, y, z;
				// COLOR0
				// 4 uint8_ts == 4 bytes
				// Offset = 12
				uint8_t b, g, r, a;
				// texture coordinates, U and V
				float u, v;
			};

		private:
			bool CreateIndexBuffer();
			bool CreateVertexBuffer();
			HRESULT GetVertexProcessingUsage(DWORD& o_usage);
#endif // EAE6320_PLATFORM_D3D

#ifdef EAE6320_PLATFORM_GL
			GLuint m_vertexArrayId;

		public:
			struct sVertex
			{
				// POSITION
				// 3 floats == 12 bytes
				// Offset = 0
				float x, y, z;
				// COLOR0
				// 4 uint8_ts == 4 bytes
				// Offset = 12
				uint8_t r, g, b, a;	// 8 bits [0,255] per RGBA channel (the alpha channel is unused but is present so that color uses a full 4 bytes)
				// texture coordinates, U and V
				float u, v;
			};
#endif // EAE6320_PLATFORM_GL

		private:
			uint32_t m_vertexCount;
			sVertex *m_vertexData;

			uint32_t m_indexCount;
			uint32_t *m_indexData;

			char* m_meshPath;
			char * m_binaryReadBuffer;

		public:
			Mesh() {};
			Mesh(char* i_meshPath);
			void DrawMesh();
			bool LoadMesh();
			void ReleaseMesh();

		private:
			bool ReadFromBinaryMeshFile(const char* const i_path);
			bool LoadVertexAndIndexData();
		};
	}
}

#endif	// EAE6320_MESH_H