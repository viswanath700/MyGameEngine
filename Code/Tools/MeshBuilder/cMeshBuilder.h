/*
	This is an example builder program that just copies the source file to the target path
	(as opposed to actually "building" it in some way so that the target is different than the source)
*/

#ifndef EAE6320_CMESHBUILDER_H
#define EAE6320_CMESHBUILDER_H

// Header Files
//=============

#include "../BuilderHelper/cbBuilder.h"
#include "../../External/Lua/Includes.h"

// Class Declaration
//==================

namespace eae6320
{
	class cMeshBuilder : public cbBuilder
	{
		// Interface
		//==========

	public:

		// Build
		//------

		virtual bool Build( const std::vector<std::string>& i_arguments );

	private:
#ifdef EAE6320_PLATFORM_D3D
		struct sVertex
		{
			float x, y, z;
			float nx, ny, nz;
			uint8_t b, g, r, a;
			float u, v;
		};
#endif // EAE6320_PLATFORM_D3D

#ifdef EAE6320_PLATFORM_GL
		struct sVertex
		{
			float x, y, z;
			float nx, ny, nz;
			uint8_t r, g, b, a;
			float u, v;
		};
#endif // EAE6320_PLATFORM_GL

		uint32_t m_vertexCount;
		sVertex *m_vertexData;

		uint32_t m_indexCount;
		uint32_t *m_indexData;

	private:
		bool LoadAsset(const char* const i_path);
		bool LoadTableValues(lua_State& io_luaState);
		bool LoadTableValues_vertices(lua_State& io_luaState);
		bool LoadTableValues_vertices_values(lua_State& io_luaState);
		bool LoadTableValues_vertices_values_position(lua_State& io_luaState, int i_vertexIndex);
		bool LoadTableValues_vertices_values_normal(lua_State& io_luaState, int i_vertexIndex);
		bool LoadTableValues_vertices_values_color(lua_State& io_luaState, int i_vertexIndex);
		bool LoadTableValues_vertices_values_texture(lua_State& io_luaState, int i_vertexIndex);
		bool LoadTableValues_indices(lua_State& io_luaState);
		bool LoadTableValues_indices_values(lua_State& io_luaState);

		bool CreateBinaryMeshFile();
	};
}

#endif	// EAE6320_CMESHBUILDER_H
