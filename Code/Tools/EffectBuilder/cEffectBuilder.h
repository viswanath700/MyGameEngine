/*
	This is an example builder program that just copies the source file to the target path
	(as opposed to actually "building" it in some way so that the target is different than the source)
*/

#ifndef EAE6320_CEFFECTBUILDER_H
#define EAE6320_CEFFECTBUILDER_H

// Header Files
//=============

#include "../BuilderHelper/cbBuilder.h"
#include "../../External/Lua/Includes.h"

// Class Declaration
//==================

namespace eae6320
{
	class cEffectBuilder : public cbBuilder
	{
		// Interface
		//==========

	public:

		// Build
		//------

		virtual bool Build( const std::vector<std::string>& i_arguments );

	private:
		char* m_vertexPath;
		char* m_fragmentPath;
		uint8_t m_renderStates;

	private:
		bool LoadAsset(const char* const i_path);
		bool LoadPathValues(lua_State& io_luaState);
		bool LoadRenderStatesValues(lua_State& io_luaState);
		bool CreateBinaryEffectFile();
	};
}

#endif	// EAE6320_CEFFECTBUILDER_H
