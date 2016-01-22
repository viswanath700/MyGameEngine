// Header Files
//=============

#include "cMaterialBuilder.h"

#include <cassert>
#include <sstream>
#include <fstream>
#include <iostream>

// Interface
//==========

// Build
//------

bool eae6320::cMaterialBuilder::Build( const std::vector<std::string>& )
{
	// Read from Lua Material file
	if (!LoadAsset(m_path_source))
	{
		return false;
	}

	// Create and write to a Bin Material file
	if (!CreateBinaryMaterialFile())
	{
		return false;
	}
	
	return true;
}

bool eae6320::cMaterialBuilder::LoadAsset(const char* const i_path)
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

	// Read Binary effect path value
	if (!LoadEffectPathValue(*luaState))
	{
		wereThereErrors = true;
	}

	// Read Uniform Data
	if (!LoadUniformDataValues(*luaState))
	{
		wereThereErrors = true;
	}

	// Read Texture Data
	if (!LoadTextureDataValues(*luaState))
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

bool eae6320::cMaterialBuilder::LoadEffectPathValue(lua_State& io_luaState)
{
	bool wereThereErrors = false;

	{
		const char* const key = "effectPath";
		lua_pushstring(&io_luaState, key);
		lua_gettable(&io_luaState, -2);

		if (!lua_isstring(&io_luaState, -1)) {
			lua_pop(&io_luaState, 1);
			wereThereErrors = true;
			std::stringstream errorMessage;
			errorMessage << "Effect Path must return a string (instead of a " <<
				luaL_typename(&io_luaState, -1) << ")\n";
			eae6320::OutputErrorMessage(errorMessage.str().c_str());
			goto OnExit;
		}

		char* tempBuffer = const_cast<char*>(lua_tostring(&io_luaState, -1));
		size_t bufferLength = std::strlen(tempBuffer) + 1;
		m_effectPath = new char[bufferLength];
		strcpy_s(m_effectPath, bufferLength, tempBuffer);
		m_effectPath[bufferLength] = '\0';

		lua_pop(&io_luaState, 1);
	}

OnExit:
	return !wereThereErrors;

}

bool eae6320::cMaterialBuilder::LoadUniformDataValues(lua_State& io_luaState)
{
	bool wereThereErrors = false;

	{
		const char* const key = "uniformData";
		lua_pushstring(&io_luaState, key);
		lua_gettable(&io_luaState, -2);

		if (!lua_istable(&io_luaState, -1))
		{
			lua_pop(&io_luaState, 1);
			wereThereErrors = true;
			std::stringstream errorMessage;
			errorMessage << "Uniform Data must return a table (instead of a " <<
				luaL_typename(&io_luaState, -1) << ")\n";
			eae6320::OutputErrorMessage(errorMessage.str().c_str());
			goto OnExit;
		}

		m_uniformCount = luaL_len(&io_luaState, -1);
		m_uniformData = new eae6320::cMaterialBuilder::sUniformData[m_uniformCount];
		m_uniformNames = new std::string[m_uniformCount];

		for (unsigned int i = 1; i <= m_uniformCount; ++i)
		{
			lua_pushinteger(&io_luaState, i);
			lua_gettable(&io_luaState, -2);

			if (lua_istable(&io_luaState, -1))
			{
				{
					const char* const key = "name";
					lua_pushstring(&io_luaState, key);
					lua_gettable(&io_luaState, -2);
					m_uniformNames[i-1] = const_cast<char*>(lua_tostring(&io_luaState, -1));
					lua_pop(&io_luaState, 1);
				}
				{
					const char* const key = "shaderType";
					lua_pushstring(&io_luaState, key);
					lua_gettable(&io_luaState, -2);
					std::string shaderTypeName = lua_tostring(&io_luaState, -1);
					if (shaderTypeName.compare("vertex") == 0)
						m_uniformData[i-1].shaderType = ShaderType::vertexShader;
					else
						m_uniformData[i-1].shaderType = ShaderType::fragmentShader;
					lua_pop(&io_luaState, 1);
				}
				{
					const char* const key = "values";
					lua_pushstring(&io_luaState, key);
					lua_gettable(&io_luaState, -2);
					m_uniformData[i-1].valueCountToSet = luaL_len(&io_luaState, -1);
					for (unsigned int j = 1; j <= m_uniformData[i-1].valueCountToSet; ++j)
					{
						lua_pushinteger(&io_luaState, j);
						lua_gettable(&io_luaState, -2);
						m_uniformData[i-1].values[j-1] = (float)lua_tonumber(&io_luaState, -1);
						lua_pop(&io_luaState, 1);
					}
					lua_pop(&io_luaState, 1);
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
				lua_pop(&io_luaState, 1);
				goto OnExit;
			}

			// Pop the Uniform value table
			lua_pop(&io_luaState, 1);
		}

		lua_pop(&io_luaState, 1);
	}

OnExit:
	return !wereThereErrors;

}

bool eae6320::cMaterialBuilder::LoadTextureDataValues(lua_State& io_luaState)
{
	bool wereThereErrors = false;

	{
		const char* const key = "textureData";
		lua_pushstring(&io_luaState, key);
		lua_gettable(&io_luaState, -2);

		if (!lua_istable(&io_luaState, -1))
		{
			lua_pop(&io_luaState, 1);
			wereThereErrors = true;
			std::stringstream errorMessage;
			errorMessage << "Texture Data must return a table (instead of a " <<
				luaL_typename(&io_luaState, -1) << ")\n";
			eae6320::OutputErrorMessage(errorMessage.str().c_str());
			goto OnExit;
		}

		m_textureCount = luaL_len(&io_luaState, -1);
		m_textureData = new eae6320::cMaterialBuilder::sTextureData[m_textureCount];

		for (unsigned int i = 1; i <= m_textureCount; ++i)
		{
			lua_pushinteger(&io_luaState, i);
			lua_gettable(&io_luaState, -2);

			if (lua_istable(&io_luaState, -1))
			{
				// Texture uniform name
				{
					const char* const key = "name";
					lua_pushstring(&io_luaState, key);
					lua_gettable(&io_luaState, -2);

					char* tempBuffer = const_cast<char*>(lua_tostring(&io_luaState, -1));
					size_t bufferLength = std::strlen(tempBuffer) + 1;
					m_textureData[i-1].uniformName = new char[bufferLength];
					strcpy_s(m_textureData[i - 1].uniformName, bufferLength, tempBuffer);
					m_textureData[i - 1].uniformName[bufferLength] = '\0';

					lua_pop(&io_luaState, 1);
				}
				// Texture path
				{
					const char* const key = "path";
					lua_pushstring(&io_luaState, key);
					lua_gettable(&io_luaState, -2);

					char* tempBuffer = const_cast<char*>(lua_tostring(&io_luaState, -1));
					size_t bufferLength = std::strlen(tempBuffer) + 1;
					m_textureData[i - 1].texturePath = new char[bufferLength];
					strcpy_s(m_textureData[i - 1].texturePath, bufferLength, tempBuffer);
					m_textureData[i - 1].texturePath[bufferLength] = '\0';

					lua_pop(&io_luaState, 1);
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
				lua_pop(&io_luaState, 1);
				goto OnExit;
			}

			// Pop the Texture value table
			lua_pop(&io_luaState, 1);
		}

		lua_pop(&io_luaState, 1);
	}

OnExit:
	return !wereThereErrors;

}

bool eae6320::cMaterialBuilder::CreateBinaryMaterialFile() {
	
	std::ofstream outputBinMaterial(m_path_target, std::ofstream::binary);

	// Effect Path
	outputBinMaterial.write(m_effectPath, std::strlen(m_effectPath));
	outputBinMaterial.write("\0", 1);

	outputBinMaterial.write((char*)&m_uniformCount, sizeof(uint32_t));

	outputBinMaterial.write((char*)m_uniformData, sizeof(sUniformData) * m_uniformCount);

	// Uniform names
	for (unsigned int i = 0; i < m_uniformCount; ++i)
	{
		outputBinMaterial.write(m_uniformNames[i].c_str(), m_uniformNames[i].size());
		outputBinMaterial.write("\0", 1);
	}

	// Texture count
	outputBinMaterial.write((char*)&m_textureCount, sizeof(uint32_t));;

	// Texture data
	for (unsigned int i = 0; i < m_textureCount; ++i)
	{
		outputBinMaterial.write(m_textureData[i].uniformName, std::strlen(m_textureData[i].uniformName));
		outputBinMaterial.write("\0", 1);

		outputBinMaterial.write(m_textureData[i].texturePath, std::strlen(m_textureData[i].texturePath));
		outputBinMaterial.write("\0", 1);
	}

	outputBinMaterial.close();
	
	return true;
}
