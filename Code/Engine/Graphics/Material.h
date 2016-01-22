/*
This file contains the function declarations for Effect
*/

#ifndef EAE6320_MATERIAL_H
#define EAE6320_MATERIAL_H

// Header Files
//=============

#include "Effect.h"
#include "Texture.h"
#include "../Math/cMatrix_transformation.h"
#include <string>

// Interface
//==========

namespace eae6320
{
	namespace Graphics
	{
		class Material
		{
		private:
			Effect m_effect;
			char* m_path_effect;

			Effect::sUniformData * m_uniformData;
			uint32_t m_uniformCount;
			std::string * m_uniformNames;

			char* m_path_material;
			char * m_binaryReadBuffer;

			Texture *m_textureList;
			uint32_t m_textureCount;
			struct sTextureData
			{
				char* uniformName;
				char* texturePath;
			};
			sTextureData *m_textureData;

		public:
			Material() {};
			Material(char* const i_path_material);
			bool LoadMaterial();
			void SetMaterial();
			void SetMaterialUniforms();
			void SetEngineUniforms(Math::cMatrix_transformation i_localToWorldTransform, CameraObject i_camera);
			void ReleaseMaterial();
			void SetTransparency(float i_alpha);

		private:
			bool ReadFromBinaryMaterialFile();
		};
	}

}

#endif	// EAE6320_MATERIAL_H