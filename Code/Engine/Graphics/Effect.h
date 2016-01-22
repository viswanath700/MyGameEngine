/*
This file contains the function declarations for Effect
*/

#ifndef EAE6320_EFFECT_H
#define EAE6320_EFFECT_H

// Header Files
//=============

#include "../Math/cVector.h"
#include "CameraObject.h"
#include "Texture.h"
#include <stdint.h>

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
		class Effect {

#ifdef EAE6320_PLATFORM_D3D
		private:
			IDirect3DVertexShader9* m_vertexShader;
			IDirect3DPixelShader9* m_fragmentShader;

			ID3DXConstantTable* m_vertexShaderConstantTable;
			ID3DXConstantTable* m_fragmentShaderConstantTable;

			char* m_shaderBuffer;

			typedef D3DXHANDLE tUniformHandle;

		public:
			typedef unsigned int tTextureUnit;

		private:
			bool ReadFromBinaryShaderFile(char* i_pathShader);

#endif // EAE6320_PLATFORM_D3D

#ifdef EAE6320_PLATFORM_GL
		public:
			GLuint m_programId;

		private:
			typedef GLint tUniformHandle;

		public:
			typedef GLint tTextureUnit;

		private:
			struct sLogInfo
			{
				GLchar* memory;
				sLogInfo(const size_t i_size) { memory = reinterpret_cast<GLchar*>(malloc(i_size)); }
				~sLogInfo() { if (memory) free(memory); }
			};

			bool LoadAndAllocateShaderProgram(const char* i_path, void*& o_shader, size_t& o_size, std::string* o_errorMessage);
			bool CreateGLProgram();
			bool LinkGLProgram();

#endif // EAE6320_PLATFORM_GL
		private:
			char* m_path_effect;
			char * m_binaryReadBuffer;

			char* m_path_vertexShader;
			char* m_path_fragmentShader;

			uint8_t *m_renderStates;

		public:
			enum ShaderType {
				vertexShader, fragmentShader
			};
			struct sUniformData
			{
				tUniformHandle uniformHandle = NULL;
				ShaderType shaderType;
				float values[4];
				uint32_t valueCountToSet;
			};

		public:
			tUniformHandle m_transformLocalToWorld;
			tUniformHandle m_transformWorldToView;
			tUniformHandle m_transformViewToScreen;


		public:
			Effect() {};
			Effect(char* const i_path_effect);

			bool CreateAndLoadEffect();
			void SetEffect();
			void SetDrawCallUniforms(eae6320::Math::cMatrix_transformation i_localToWorldTransform, CameraObject i_camera);

			tUniformHandle GetUniformHandle(const char* i_uniformName, bool i_fromVertexShader);
			void SetUniformHandle(sUniformData i_uniformData, bool i_fromVertexShader);

			Texture::tSampler2D GetSampler2DID(const char* i_uniformName);
			void SetSampler2DID(Texture::tSampler2D i_sampler, Texture::tTextureHandle i_textureHandle, tTextureUnit i_textureUnit);

			void ReleaseEffect();

		private:
			bool ReadFromBinaryEffectFile();
			bool LoadVertexShader();
			bool LoadFragmentShader();
			void SetRenderStates();
		};
	}
}

#endif	// EAE6320_EFFECT_H