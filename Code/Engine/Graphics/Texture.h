/*
This file contains the function declarations for Effect
*/

#ifndef EAE6320_TEXTURE_H
#define EAE6320_TEXTURE_H

// Header Files
//=============

#ifdef EAE6320_PLATFORM_D3D
#include <d3d9.h>
#include <d3dx9shader.h>
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
		class Texture
		{
#ifdef EAE6320_PLATFORM_D3D
		public:
			typedef IDirect3DTexture9* tTextureHandle;
			typedef DWORD tSampler2D;
#endif // EAE6320_PLATFORM_D3D

#ifdef EAE6320_PLATFORM_GL
		public:
			typedef GLuint tTextureHandle;
			typedef GLint tSampler2D;
#endif // EAE6320_PLATFORM_GL

		private:
			char * m_texturePath;
		
		public:
			tTextureHandle m_texture;
			tSampler2D m_samplerID;

		public:
			Texture() {};
			Texture(char* const i_path_texture);
			bool LoadTexture();
			void ReleaseTexture();
		};
	}

}

#endif	// EAE6320_TEXTURE_H