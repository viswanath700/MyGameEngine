// Header Files
//=============

#include "cEffectBuilder.h"

#include <cassert>
#include <sstream>
#include <fstream>
#include "../../Engine/Windows/WindowsFunctions.h"

// Interface
//==========

enum RenderStates : uint8_t
{
	AlphaTransparency = 1 << 0,
	DepthTest = 1 << 1,
	DepthWrite = 1 << 2,
	FaceCulling = 1 << 3,
};

// Build
//------

bool eae6320::cEffectBuilder::Build( const std::vector<std::string>& )
{
	// Read from Lua Effect file
	if (!LoadAsset(m_path_source))
	{
		return false;
	}

	// Create and write to a Bin Effect file
	if (!CreateBinaryEffectFile())
	{
		return false;
	}
	
	return true;
}

bool eae6320::cEffectBuilder::LoadAsset(const char* const i_path)
{
	bool wereThereErrors = false;

	// Create a new Lua state
	lua_State* luaState = NULL;
	{
		luaState = luaL_newstate();
		if (!luaState)
		{
			wereThereErrors = true;
			eae6320::OutputErrorMessage("Failed to create a new Lua state");
			goto OnExit;
		}
	}

	// Load the asset file as a "chunk",
	// meaning there will be a callable function at the top of the stack
	{
		const int luaResult = luaL_loadfile(luaState, i_path);
		if (luaResult != LUA_OK)
		{
			wereThereErrors = true;
			eae6320::OutputErrorMessage(lua_tostring(luaState, -1));
			// Pop the error message
			lua_pop(luaState, 1);
			goto OnExit;
		}
	}
	// Execute the "chunk", which should load the asset
	// into a table at the top of the stack
	{
		const int argumentCount = 0;
		const int returnValueCount = LUA_MULTRET;	// Return _everything_ that the file returns
		const int noMessageHandler = 0;
		const int luaResult = lua_pcall(luaState, argumentCount, returnValueCount, noMessageHandler);
		if (luaResult == LUA_OK)
		{
			// A well-behaved asset file will only return a single value
			const int returnedValueCount = lua_gettop(luaState);
			if (returnedValueCount == 1)
			{
				// A correct asset file _must_ return a table
				if (!lua_istable(luaState, -1))
				{
					wereThereErrors = true;
					std::stringstream errorMessage;
					errorMessage << "Asset files must return a table (instead of a " <<
						luaL_typename(luaState, -1) << ")\n";
					eae6320::OutputErrorMessage(errorMessage.str().c_str());
					// Pop the returned non-table value
					lua_pop(luaState, 1);
					goto OnExit;
				}
			}
			else
			{
				wereThereErrors = true;
				std::stringstream errorMessage;
				errorMessage << "Asset files must return a single table (instead of " <<
					returnedValueCount << " values)"
					"\n";
				eae6320::OutputErrorMessage(errorMessage.str().c_str());
				// Pop every value that was returned
				lua_pop(luaState, returnedValueCount);
				goto OnExit;
			}
		}
		else
		{
			wereThereErrors = true;
			eae6320::OutputErrorMessage(lua_tostring(luaState, -1));
			// Pop the error message
			lua_pop(luaState, 1);
			goto OnExit;
		}
	}

	// If this code is reached the asset file was loaded successfully,
	// and its table is now at index -1
	if (lua_isnil(luaState, -1)) {
		lua_pop(luaState, 1);
		wereThereErrors = true;
		std::stringstream errorMessage;
		errorMessage << "Asset files must not return null.\n";
		eae6320::OutputErrorMessage(errorMessage.str().c_str());
		goto OnExit;
	}

	// Read Path values from Lua file
	if (!LoadPathValues(*luaState))
	{
		wereThereErrors = true;
	}

	// Read Render State values from Lua file
	if (!LoadRenderStatesValues(*luaState))
	{
		wereThereErrors = true;
	}

	// Pop the table
	lua_pop(luaState, 1);

OnExit:

	if (luaState)
	{
		// If I haven't made any mistakes
		// there shouldn't be anything on the stack,
		// regardless of any errors encountered while loading the file:
		assert(lua_gettop(luaState) == 0);

		lua_close(luaState);
		luaState = NULL;
	}

	return !wereThereErrors;
}

bool eae6320::cEffectBuilder::LoadPathValues(lua_State& io_luaState)
{
	bool wereThereErrors = false;

	{
		const char* const key = "vertexShaderPath";
		lua_pushstring(&io_luaState, key);
		lua_gettable(&io_luaState, -2);

		if (!lua_isstring(&io_luaState, -1)) {
			lua_pop(&io_luaState, 1);
			wereThereErrors = true;
			std::stringstream errorMessage;
			errorMessage << "Asset files must return a string (instead of a " <<
				luaL_typename(&io_luaState, -1) << ")\n";
			eae6320::OutputErrorMessage(errorMessage.str().c_str());
			goto OnExit;
		}

		char* tempBuffer = const_cast<char*>(lua_tostring(&io_luaState, -1));
		size_t bufferLength = std::strlen(tempBuffer) + 1;
		m_vertexPath = new char[bufferLength];
		strcpy_s(m_vertexPath, bufferLength, tempBuffer);
		m_vertexPath[bufferLength] = '\0';

		lua_pop(&io_luaState, 1);
	}

	{
		const char* const key = "fragmentShaderPath";
		lua_pushstring(&io_luaState, key);
		lua_gettable(&io_luaState, -2);

		if (!lua_isstring(&io_luaState, -1)) {
			lua_pop(&io_luaState, 1);
			wereThereErrors = true;
			std::stringstream errorMessage;
			errorMessage << "Asset files must return a string (instead of a " <<
				luaL_typename(&io_luaState, -1) << ")\n";
			eae6320::OutputErrorMessage(errorMessage.str().c_str());
			goto OnExit;
		}

		char* tempBuffer = const_cast<char*>(lua_tostring(&io_luaState, -1));
		size_t bufferLength = std::strlen(tempBuffer) + 1;
		m_fragmentPath = new char[bufferLength];
		strcpy_s(m_fragmentPath, bufferLength, tempBuffer);
		m_fragmentPath[bufferLength] = '\0';

		lua_pop(&io_luaState, 1);
	}

OnExit:
	return !wereThereErrors;

}

bool eae6320::cEffectBuilder::LoadRenderStatesValues(lua_State& io_luaState)
{
	bool wereThereErrors = false;

	{
		const char* const key = "renderStates";
		lua_pushstring(&io_luaState, key);
		lua_gettable(&io_luaState, -2);

		if (!lua_istable(&io_luaState, -1))
		{
			lua_pop(&io_luaState, 1);
			wereThereErrors = true;
			std::stringstream errorMessage;
			errorMessage << "Render States must return a table (instead of a " <<
				luaL_typename(&io_luaState, -1) << ")\n";
			eae6320::OutputErrorMessage(errorMessage.str().c_str());
			goto OnExit;
		}

		// Setting Alpha Trransparency from Lua
		{
			const char* const key = "alphaTransparency";
			lua_pushstring(&io_luaState, key);
			lua_gettable(&io_luaState, -2);

			if (lua_isnil(&io_luaState, -1))
				m_renderStates = 0;
			else if (lua_toboolean(&io_luaState, -1))
				m_renderStates = AlphaTransparency;
			else
				m_renderStates = 0;

			lua_pop(&io_luaState, 1);
		}

		// Setting Depth Testing from Lua
		{
			const char* const key = "depthTesting";
			lua_pushstring(&io_luaState, key);
			lua_gettable(&io_luaState, -2);

			if (lua_isnil(&io_luaState, -1))
				m_renderStates |= DepthTest;
			else if (lua_toboolean(&io_luaState, -1))
				m_renderStates |= DepthTest;

			lua_pop(&io_luaState, 1);
		}

		// Setting Depth Writing from Lua
		{
			const char* const key = "depthWriting";
			lua_pushstring(&io_luaState, key);
			lua_gettable(&io_luaState, -2);

			if (lua_isnil(&io_luaState, -1))
				m_renderStates |= DepthWrite;
			else if (lua_toboolean(&io_luaState, -1))
				m_renderStates |= DepthWrite;

			lua_pop(&io_luaState, 1);
		}

		// Setting Face Culling from Lua
		{
			const char* const key = "faceCulling";
			lua_pushstring(&io_luaState, key);
			lua_gettable(&io_luaState, -2);

			if (lua_isnil(&io_luaState, -1))
				m_renderStates |= FaceCulling;
			else if (lua_toboolean(&io_luaState, -1))
				m_renderStates |= FaceCulling;

			lua_pop(&io_luaState, 1);
		}

		lua_pop(&io_luaState, 1);
	}

OnExit:
	return !wereThereErrors;

}

bool eae6320::cEffectBuilder::CreateBinaryEffectFile() {
	
	std::ofstream outputBinEffect(m_path_target, std::ofstream::binary);

	outputBinEffect.write(m_vertexPath, std::strlen(m_vertexPath));
	outputBinEffect.write("\0", 1);
	outputBinEffect.write(m_fragmentPath, std::strlen(m_fragmentPath));
	outputBinEffect.write("\0", 1);

	outputBinEffect.write((char*)&m_renderStates, sizeof(uint8_t));

	outputBinEffect.close();
	
	return true;
}
