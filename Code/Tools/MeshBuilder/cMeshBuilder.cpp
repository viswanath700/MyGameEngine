// Header Files
//=============

#include "cMeshBuilder.h"

#include <cassert>
#include <sstream>
#include <fstream>
#include "../../Engine/Windows/WindowsFunctions.h"

// Interface
//==========

// Build
//------

bool eae6320::cMeshBuilder::Build( const std::vector<std::string>& )
{
	// Read from Lua Mesh file
	if (!LoadAsset(m_path_source))
	{
		return false;
	}

	// Create and write to a Bin Mesh file
	if (!CreateBinaryMeshFile())
	{
		return false;
	}

	// Deleting the pointers to the vertexData and indexData 
	{
		delete[] m_vertexData;
		m_vertexData = NULL;

		delete[] m_indexData;
		m_indexData = NULL;
	}
	
	return true;
}

bool eae6320::cMeshBuilder::LoadAsset(const char* const i_path)
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
	if (!LoadTableValues(*luaState))
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

bool eae6320::cMeshBuilder::LoadTableValues(lua_State& io_luaState)
{
	if (!LoadTableValues_vertices(io_luaState))
	{
		return false;
	}
	if (!LoadTableValues_indices(io_luaState))
	{
		return false;
	}

	return true;
}

bool eae6320::cMeshBuilder::LoadTableValues_vertices(lua_State& io_luaState) {
	bool wereThereErrors = false;

	const char* const key = "vertices";
	lua_pushstring(&io_luaState, key);
	lua_gettable(&io_luaState, -2);

	if (lua_istable(&io_luaState, -1))
	{
		if (!LoadTableValues_vertices_values(io_luaState))
		{
			wereThereErrors = true;
			goto OnExit;
		}
	}
	else
	{
		wereThereErrors = true;
		std::stringstream errorMessage;
		errorMessage << "The value at \"" << key << "\" must be a table "
			"(instead of a " << luaL_typename(&io_luaState, -1) << ")\n";
		eae6320::OutputErrorMessage(errorMessage.str().c_str());
		goto OnExit;
	}

OnExit:

	// Pop the vertices table
	lua_pop(&io_luaState, 1);

	return !wereThereErrors;
}

bool eae6320::cMeshBuilder::LoadTableValues_vertices_values(lua_State& io_luaState)
{
	m_vertexCount = luaL_len(&io_luaState, -1);
	m_vertexData = new eae6320::cMeshBuilder::sVertex[m_vertexCount];
	for (unsigned int i = 1; i <= m_vertexCount; ++i)
	{
		lua_pushinteger(&io_luaState, i);
		lua_gettable(&io_luaState, -2);

		if (lua_istable(&io_luaState, -1))
		{
			if (!LoadTableValues_vertices_values_position(io_luaState, i - 1))
			{
				return false;
			}
			if (!LoadTableValues_vertices_values_color(io_luaState, i - 1))
			{
				return false;
			}
			if (!LoadTableValues_vertices_values_texture(io_luaState, i - 1))
			{
				return false;
			}
		}
		else
		{
			std::stringstream errorMessage;
			errorMessage << "The value at \"" << i << "\" must be a table "
				"(instead of a " << luaL_typename(&io_luaState, -1) << ")\n";
			eae6320::OutputErrorMessage(errorMessage.str().c_str());
			// Pop the vertex value table
			lua_pop(&io_luaState, 1);
			return false;
		}

		// Pop the vertex value table
		lua_pop(&io_luaState, 1);
	}

	return true;
}

bool eae6320::cMeshBuilder::LoadTableValues_vertices_values_position(lua_State& io_luaState, int i_vertexIndex)
{
	bool wereThereErrors = false;

	const char* const key = "position";
	lua_pushstring(&io_luaState, key);
	lua_gettable(&io_luaState, -2);
	if (lua_istable(&io_luaState, -1))
	{
		// x
		lua_pushinteger(&io_luaState, 1);
		lua_gettable(&io_luaState, -2);
		m_vertexData[i_vertexIndex].x = (float)lua_tonumber(&io_luaState, -1);
		lua_pop(&io_luaState, 1);

		// y
		lua_pushinteger(&io_luaState, 2);
		lua_gettable(&io_luaState, -2);
		m_vertexData[i_vertexIndex].y = (float)lua_tonumber(&io_luaState, -1);
		lua_pop(&io_luaState, 1);

		// z
		lua_pushinteger(&io_luaState, 3);
		lua_gettable(&io_luaState, -2);
		m_vertexData[i_vertexIndex].z = (float)lua_tonumber(&io_luaState, -1);
		lua_pop(&io_luaState, 1);
	}
	else
	{
		wereThereErrors = true;
		std::stringstream errorMessage;
		errorMessage << "The value at \"" << key << "\" must be a table "
			"(instead of a " << luaL_typename(&io_luaState, -1) << ")\n";
		eae6320::OutputErrorMessage(errorMessage.str().c_str());
		goto OnExit;
	}

OnExit:

	// Pop the parameters table
	lua_pop(&io_luaState, 1);

	return !wereThereErrors;
}

bool eae6320::cMeshBuilder::LoadTableValues_vertices_values_color(lua_State& io_luaState, int i_vertexIndex)
{
	bool wereThereErrors = false;

	const char* const key = "color";
	lua_pushstring(&io_luaState, key);
	lua_gettable(&io_luaState, -2);
	if (lua_istable(&io_luaState, -1))
	{
		// r
		lua_pushinteger(&io_luaState, 1);
		lua_gettable(&io_luaState, -2);
		m_vertexData[i_vertexIndex].r = (uint8_t)(lua_tonumber(&io_luaState, -1) * 255);
		lua_pop(&io_luaState, 1);

		// g
		lua_pushinteger(&io_luaState, 2);
		lua_gettable(&io_luaState, -2);
		m_vertexData[i_vertexIndex].g = (uint8_t)(lua_tonumber(&io_luaState, -1) * 255);
		lua_pop(&io_luaState, 1);

		// b
		lua_pushinteger(&io_luaState, 3);
		lua_gettable(&io_luaState, -2);
		m_vertexData[i_vertexIndex].b = (uint8_t)(lua_tonumber(&io_luaState, -1) * 255);
		lua_pop(&io_luaState, 1);

		// a
		lua_pushinteger(&io_luaState, 4);
		lua_gettable(&io_luaState, -2);
		m_vertexData[i_vertexIndex].a = (uint8_t)(lua_tonumber(&io_luaState, -1) * 255);
		lua_pop(&io_luaState, 1);
	}
	else
	{
		wereThereErrors = true;
		std::stringstream errorMessage;
		errorMessage << "The value at \"" << key << "\" must be a table "
			"(instead of a " << luaL_typename(&io_luaState, -1) << ")\n";
		eae6320::OutputErrorMessage(errorMessage.str().c_str());
		goto OnExit;
	}

OnExit:

	// Pop the parameters table
	lua_pop(&io_luaState, 1);

	return !wereThereErrors;
}

bool eae6320::cMeshBuilder::LoadTableValues_vertices_values_texture(lua_State& io_luaState, int i_vertexIndex)
{
	bool wereThereErrors = false;

	const char* const key = "texture";
	lua_pushstring(&io_luaState, key);
	lua_gettable(&io_luaState, -2);
	if (lua_istable(&io_luaState, -1))
	{
		// u
		lua_pushinteger(&io_luaState, 1);
		lua_gettable(&io_luaState, -2);
		m_vertexData[i_vertexIndex].u = (float)lua_tonumber(&io_luaState, -1);
		lua_pop(&io_luaState, 1);

		// v
		lua_pushinteger(&io_luaState, 2);
		lua_gettable(&io_luaState, -2);
		// We will be using D3D convention for both platfomes for V coordinate
		m_vertexData[i_vertexIndex].v = ( 1.0f - (float)lua_tonumber(&io_luaState, -1) );
		lua_pop(&io_luaState, 1);
	}
	else
	{
		wereThereErrors = true;
		std::stringstream errorMessage;
		errorMessage << "The value at \"" << key << "\" must be a table "
			"(instead of a " << luaL_typename(&io_luaState, -1) << ")\n";
		eae6320::OutputErrorMessage(errorMessage.str().c_str());
		goto OnExit;
	}

OnExit:

	// Pop the parameters table
	lua_pop(&io_luaState, 1);

	return !wereThereErrors;
}

bool eae6320::cMeshBuilder::LoadTableValues_indices(lua_State& io_luaState) {
	bool wereThereErrors = false;

	const char* const key = "indices";
	lua_pushstring(&io_luaState, key);
	lua_gettable(&io_luaState, -2);

	if (lua_istable(&io_luaState, -1))
	{
		if (!LoadTableValues_indices_values(io_luaState))
		{
			wereThereErrors = true;
			goto OnExit;
		}
	}
	else
	{
		wereThereErrors = true;
		std::stringstream errorMessage;
		errorMessage << "The value at \"" << key << "\" must be a table "
			"(instead of a " << luaL_typename(&io_luaState, -1) << ")\n";
		eae6320::OutputErrorMessage(errorMessage.str().c_str());
		goto OnExit;
	}

OnExit:

	// Pop the textures table
	lua_pop(&io_luaState, 1);

	return !wereThereErrors;
}

bool eae6320::cMeshBuilder::LoadTableValues_indices_values(lua_State& io_luaState)
{
	const unsigned int triangleCount = luaL_len(&io_luaState, -1);
	const unsigned int vertexCountPerTriangle = 3;

	m_indexCount = triangleCount * vertexCountPerTriangle;

	m_indexData = new uint32_t[m_indexCount];
	int k = 0;

	for (unsigned int i = 1; i <= triangleCount; ++i)
	{
		lua_pushinteger(&io_luaState, i);
		lua_gettable(&io_luaState, -2);

		if (lua_istable(&io_luaState, -1))
		{
			for (unsigned int j = 1; j <= vertexCountPerTriangle; ++j)
			{
				lua_pushinteger(&io_luaState, j);
				lua_gettable(&io_luaState, -2);
				m_indexData[k] = (uint32_t)lua_tointeger(&io_luaState, -1);
				lua_pop(&io_luaState, 1);
				k++;
			}
		}
		else
		{
			std::stringstream errorMessage;
			errorMessage << "The value at \"" << i << "\" must be a table "
				"(instead of a " << luaL_typename(&io_luaState, -1) << ")\n";
			eae6320::OutputErrorMessage(errorMessage.str().c_str());
			// Pop the vertex value table
			lua_pop(&io_luaState, 1);
			return false;
		}

		// Pop the vertex value table
		lua_pop(&io_luaState, 1);
	}

	return true;
}

bool eae6320::cMeshBuilder::CreateBinaryMeshFile() {

	// Changing vertex order for d3d
#ifdef EAE6320_PLATFORM_D3D
	for (unsigned int i = 0; i < m_indexCount; i += 3) {
		uint32_t temp = m_indexData[i + 1];
		m_indexData[i + 1] = m_indexData[i + 2];
		m_indexData[i + 2] = temp;
	}
#endif // EAE6320_PLATFORM_D3D
	
	std::ofstream outputBinMesh(m_path_target, std::ofstream::binary);

	outputBinMesh.write((char*)&m_vertexCount, sizeof(uint32_t));
	outputBinMesh.write((char*)&m_indexCount, sizeof(uint32_t));

	outputBinMesh.write((char*)m_vertexData, sizeof(sVertex) * m_vertexCount);
	outputBinMesh.write((char*)m_indexData, sizeof(uint32_t) * m_indexCount);

	outputBinMesh.close();
	
	return true;
}
