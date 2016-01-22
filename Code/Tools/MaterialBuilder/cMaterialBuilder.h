/*
	This is an example builder program that just copies the source file to the target path
	(as opposed to actually "building" it in some way so that the target is different than the source)
*/

#ifndef EAE6320_CMATERIALBUILDER_H
#define EAE6320_CMATERIALBUILDER_H

// Header Files
//=============

#include "../BuilderHelper/cbBuilder.h"
#include "../../External/Lua/Includes.h"
#include "../../External/OpenGlExtensions/OpenGlExtensions.h"
#include <string>

// Class Declaration
//==================

namespace eae6320
{
	class cMaterialBuilder : public cbBuilder
	{
		// Interface
		//==========

	public:

		// Build
		//------

		virtual bool Build( const std::vector<std::string>& i_arguments );

		typedef
#if defined( EAE6320_PLATFORM_D3D )
			// This is conceptually a D3DXHANDLE but is defined this way
			// so that external files aren't required to specify the #include path to the DirectX SDK
			const char*
#elif defined( EAE6320_PLATFORM_GL )
			GLint
#endif
			tUniformHandle;

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

		struct sTextureData
		{
			char* uniformName;
			char* texturePath;
		};

	private:
		char* m_effectPath;
		sUniformData * m_uniformData;
		uint32_t m_uniformCount;
		std::string * m_uniformNames;

		sTextureData * m_textureData;
		uint32_t m_textureCount;

	private:
		bool LoadAsset(const char* const i_path);
		bool LoadEffectPathValue(lua_State& io_luaState);
		bool LoadUniformDataValues(lua_State& io_luaState);
		bool LoadTextureDataValues(lua_State& io_luaState);
		bool CreateBinaryMaterialFile();
	};
}

#endif	// EAE6320_CMATERIALBUILDER_H
