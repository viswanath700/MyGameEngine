// Header Files
//=============

#include "Material.h"

#include <cassert>
#include <sstream>
#include <fstream>

// Interface
//==========

eae6320::Graphics::Material::Material(char* const i_path_material)
{
	m_path_material = i_path_material;
	ReadFromBinaryMaterialFile();
	m_effect = Effect(m_path_effect);

	m_textureList = new Texture[m_textureCount];
	for (unsigned int i = 0; i < m_textureCount; i++)
	{
		m_textureList[i] = Texture(m_textureData[i].texturePath);
	}
}

bool eae6320::Graphics::Material::LoadMaterial()
{
	if (!m_effect.CreateAndLoadEffect())
	{
		return false;
	}

	for (size_t i = 0; i < m_uniformCount; ++i)
	{
		bool fromVertexShader = true;

		if (m_uniformData[i].shaderType == Effect::ShaderType::fragmentShader)
			fromVertexShader = false;

		m_uniformData[i].uniformHandle = m_effect.GetUniformHandle(m_uniformNames[i].c_str(), fromVertexShader);
	}

	for (size_t i = 0; i < m_textureCount; ++i)
	{
		if (!m_textureList[i].LoadTexture())
		{
			return false;
		}

		m_textureList[i].m_samplerID = m_effect.GetSampler2DID(m_textureData[i].uniformName);
	}

	return true;
}

bool eae6320::Graphics::Material::ReadFromBinaryMaterialFile()
{
	std::ifstream inputBinaryEffectFile(m_path_material, std::ifstream::binary);

	if (inputBinaryEffectFile) {

		// Get length of the file
		inputBinaryEffectFile.seekg(0, inputBinaryEffectFile.end);
		size_t fileLength = (size_t)inputBinaryEffectFile.tellg();
		inputBinaryEffectFile.seekg(0, inputBinaryEffectFile.beg);

		// Read and store into buffer
		m_binaryReadBuffer = new char[fileLength];
		inputBinaryEffectFile.read(m_binaryReadBuffer, fileLength);

		// Extracting corresponding info after reading binary
		if (inputBinaryEffectFile)
		{
			m_path_effect = m_binaryReadBuffer;
			m_uniformCount = *reinterpret_cast<uint32_t*>(m_binaryReadBuffer + std::strlen(m_path_effect) + 1);
			m_uniformData = reinterpret_cast<Effect::sUniformData*>(m_binaryReadBuffer + std::strlen(m_path_effect) + 1 + sizeof(uint32_t));

			m_uniformNames = new std::string[m_uniformCount];

			size_t prevStringLength = 0;
			for (size_t i = 0; i < m_uniformCount; ++i)
			{
				m_uniformNames[i] = m_binaryReadBuffer + std::strlen(m_path_effect) + 1 + sizeof(uint32_t) + (sizeof(Effect::sUniformData) * m_uniformCount) + prevStringLength;
				prevStringLength += m_uniformNames[i].size() + 1;
			}

			// Extracting Texture data and initialising the texture
			m_textureCount = *reinterpret_cast<uint32_t*>(m_binaryReadBuffer + std::strlen(m_path_effect) + 1 + sizeof(uint32_t) + (sizeof(Effect::sUniformData) * m_uniformCount) + prevStringLength);
			m_textureData = new sTextureData[m_textureCount];
			for (size_t i = 0; i < m_textureCount; ++i)
			{
				m_textureData[i].uniformName = m_binaryReadBuffer + std::strlen(m_path_effect) + 1 + sizeof(uint32_t) + (sizeof(Effect::sUniformData) * m_uniformCount) + prevStringLength + sizeof(uint32_t);
				prevStringLength += std::strlen(m_textureData[i].uniformName) + 1;

				m_textureData[i].texturePath = m_binaryReadBuffer + std::strlen(m_path_effect) + 1 + sizeof(uint32_t) + (sizeof(Effect::sUniformData) * m_uniformCount) + prevStringLength + sizeof(uint32_t);
				prevStringLength += std::strlen(m_textureData[i].texturePath) + 1;
			}
			
		}

		return true;
	}

	return false;
}

void eae6320::Graphics::Material::SetMaterial()
{
	m_effect.SetEffect();
}

void eae6320::Graphics::Material::SetMaterialUniforms()
{
	for (size_t i = 0; i < m_uniformCount; ++i)
	{
		bool fromVertexShader = true;

		if (m_uniformData[i].shaderType == Effect::ShaderType::fragmentShader)
			fromVertexShader = false;

		m_effect.SetUniformHandle(m_uniformData[i], fromVertexShader);
	}

	for (size_t i = 0; i < m_textureCount; ++i)
	{
		m_effect.SetSampler2DID(m_textureList[i].m_samplerID, m_textureList[i].m_texture, (Effect::tTextureUnit)i);
	}
}

void eae6320::Graphics::Material::SetEngineUniforms(Math::cMatrix_transformation i_localToWorldTransform, CameraObject i_camera)
{
	m_effect.SetDrawCallUniforms(i_localToWorldTransform, i_camera);
}

void eae6320::Graphics::Material::ReleaseMaterial()
{
	//m_texture.ReleaseTexture();
	m_effect.ReleaseEffect();
}

void eae6320::Graphics::Material::SetTransparency(float i_alpha)
{
	// Assuming alpha modifier uniform is always at the index 1
	if(m_uniformCount > 1)
		m_uniformData[1].values[0] = i_alpha;
}