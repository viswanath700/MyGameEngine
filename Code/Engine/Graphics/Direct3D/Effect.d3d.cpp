// Header Files
//=============
#include "../Effect.h"
#include "../Graphics.h"

#include <cassert>
#include <sstream>
#include <fstream>
#include "../../UserOutput/UserOutput.h"
#include "../../Math/cMatrix_transformation.h"

// Interface
//==========

enum RenderStates : uint8_t
{
	AlphaTransparency = 1 << 0,
	DepthTest = 1 << 1,
	DepthWrite = 1 << 2,
	FaceCulling = 1 << 3,
};

eae6320::Graphics::Effect::Effect(char* const i_path_effect)
{
	m_vertexShader = NULL;
	m_fragmentShader = NULL;

	m_transformViewToScreen = NULL;
	m_transformWorldToView = NULL;
	m_transformViewToScreen = NULL;
	m_vertexShaderConstantTable = NULL;
	m_fragmentShaderConstantTable = NULL;
	
	m_shaderBuffer = NULL;

	m_path_effect = i_path_effect;
	m_path_vertexShader = NULL;
	m_path_fragmentShader = NULL;

	m_renderStates = 0;
}

bool eae6320::Graphics::Effect::CreateAndLoadEffect()
{
	if (!ReadFromBinaryEffectFile())
	{
		return false;
	}
	if (!LoadVertexShader())
	{
		return false;
	}
	if (!LoadFragmentShader())
	{
		return false;
	}

	return true;
}

void eae6320::Graphics::Effect::SetEffect() {
	HRESULT result;
	// Set the vertex and fragment shaders
	{
		result = Graphics::GetLocalDirect3dDevice()->SetVertexShader(m_vertexShader);
		assert(SUCCEEDED(result));
		result = Graphics::GetLocalDirect3dDevice()->SetPixelShader(m_fragmentShader);
		assert(SUCCEEDED(result));
	}

	SetRenderStates();
}

void eae6320::Graphics::Effect::SetDrawCallUniforms(eae6320::Math::cMatrix_transformation i_localToWorldTransform, CameraObject i_camera)
{
	HRESULT result;
	{
		Math::cMatrix_transformation i_worldToViewTransform = Math::cMatrix_transformation::CreateWorldToViewTransform(i_camera.m_orientation, i_camera.m_position);
		Math::cMatrix_transformation i_viewToScreenTransform = Math::cMatrix_transformation::CreateViewToScreenTransform(i_camera.m_fieldOfView_y, i_camera.m_aspectRatio, i_camera.m_z_nearPlane, i_camera.m_z_farPlane);
		
		result = m_vertexShaderConstantTable->SetMatrixTranspose(eae6320::Graphics::GetLocalDirect3dDevice(), m_transformLocalToWorld, reinterpret_cast<const D3DXMATRIX*>(&i_localToWorldTransform));
		assert(SUCCEEDED(result));
		result = m_vertexShaderConstantTable->SetMatrixTranspose(eae6320::Graphics::GetLocalDirect3dDevice(), m_transformWorldToView, reinterpret_cast<const D3DXMATRIX*>(&i_worldToViewTransform));
		assert(SUCCEEDED(result));
		result = m_vertexShaderConstantTable->SetMatrixTranspose(eae6320::Graphics::GetLocalDirect3dDevice(), m_transformViewToScreen, reinterpret_cast<const D3DXMATRIX*>(&i_viewToScreenTransform));
		assert(SUCCEEDED(result));
	}
}

void eae6320::Graphics::Effect::ReleaseEffect()
{
	if ( m_vertexShader )
	{
		m_vertexShader->Release();
		m_vertexShader = NULL;
	}
	if ( m_fragmentShader )
	{
		m_fragmentShader->Release();
		m_fragmentShader = NULL;
	}
	if (m_vertexShaderConstantTable)
	{
		m_vertexShaderConstantTable->Release();
		m_vertexShaderConstantTable = NULL;
	}
	if (m_fragmentShaderConstantTable)
	{
		m_fragmentShaderConstantTable->Release();
		m_fragmentShaderConstantTable = NULL;
	}
	//if (m_binaryReadBuffer)
	//{
	//	delete[] m_binaryReadBuffer;
	//	m_binaryReadBuffer = NULL;
	//}
}

bool eae6320::Graphics::Effect::LoadVertexShader() {
	// Load the source code from file and compile it
	if (!ReadFromBinaryShaderFile(m_path_vertexShader)) {
		return false;
	}
	D3DXGetShaderConstantTable(reinterpret_cast<const DWORD*>(m_shaderBuffer), &m_vertexShaderConstantTable);

	m_transformLocalToWorld = m_vertexShaderConstantTable->GetConstantByName(NULL, "g_transform_localToWorld");
	m_transformWorldToView = m_vertexShaderConstantTable->GetConstantByName(NULL, "g_transform_worldToView");
	m_transformViewToScreen = m_vertexShaderConstantTable->GetConstantByName(NULL, "g_transform_viewToScreen");

	// Create the vertex shader object
	bool wereThereErrors = false;
	{
		HRESULT result = Graphics::GetLocalDirect3dDevice()->CreateVertexShader(reinterpret_cast<DWORD*>(m_shaderBuffer),
			&m_vertexShader);
		if (FAILED(result))
		{
			eae6320::UserOutput::Print("Direct3D failed to create the vertex shader");
			wereThereErrors = true;
		}
		delete[] m_shaderBuffer;
	}
	return !wereThereErrors;
}

bool eae6320::Graphics::Effect::LoadFragmentShader() {
	// Load the source code from file and compile it
	if (!ReadFromBinaryShaderFile(m_path_fragmentShader)) {
		return false;
	}
	D3DXGetShaderConstantTable(reinterpret_cast<const DWORD*>(m_shaderBuffer), &m_fragmentShaderConstantTable);

	// Create the fragment shader object
	bool wereThereErrors = false;
	{
		HRESULT result = Graphics::GetLocalDirect3dDevice()->CreatePixelShader(reinterpret_cast<DWORD*>(m_shaderBuffer),
			&m_fragmentShader);
		if (FAILED(result))
		{
			eae6320::UserOutput::Print("Direct3D failed to create the fragment shader");
			wereThereErrors = true;
		}
		delete[] m_shaderBuffer;
	}
	return !wereThereErrors;
}

bool eae6320::Graphics::Effect::ReadFromBinaryShaderFile(char* i_pathShader)
{
	std::ifstream inputBinaryShaderFile(i_pathShader, std::ifstream::binary);

	if (inputBinaryShaderFile) {

		// Get length of the file
		inputBinaryShaderFile.seekg(0, inputBinaryShaderFile.end);
		size_t fileLength = (size_t)inputBinaryShaderFile.tellg();
		inputBinaryShaderFile.seekg(0, inputBinaryShaderFile.beg);

		// Read and store into buffer
		m_shaderBuffer = new char[fileLength];
		inputBinaryShaderFile.read(m_shaderBuffer, fileLength);

		return true;
	}

	return false;
}

void eae6320::Graphics::Effect::SetRenderStates()
{
	// Alpha Transparency
	{
		if ((*m_renderStates) & AlphaTransparency)
		{
			Graphics::GetLocalDirect3dDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
			Graphics::GetLocalDirect3dDevice()->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
			Graphics::GetLocalDirect3dDevice()->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
		}
		else
		{
			Graphics::GetLocalDirect3dDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
		}
	}

	// Depth Testing
	{
		if ((*m_renderStates) & DepthTest)
		{
			Graphics::GetLocalDirect3dDevice()->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
			Graphics::GetLocalDirect3dDevice()->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
		}
		else
		{
			Graphics::GetLocalDirect3dDevice()->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
		}
	}

	// Depth Writing
	{
		if ((*m_renderStates) & DepthWrite)
		{
			Graphics::GetLocalDirect3dDevice()->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
		}
		else
		{
			Graphics::GetLocalDirect3dDevice()->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
		}
	}

	// Face Culling
	{
		if ((*m_renderStates) & FaceCulling)
		{
			Graphics::GetLocalDirect3dDevice()->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
		}
		else
		{
			Graphics::GetLocalDirect3dDevice()->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
		}
	}
}

D3DXHANDLE eae6320::Graphics::Effect::GetUniformHandle(const char* i_uniformName, bool i_fromVertexShader)
{
	ID3DXConstantTable* shaderConstantTable = i_fromVertexShader ? m_vertexShaderConstantTable : m_fragmentShaderConstantTable;

	return shaderConstantTable->GetConstantByName(NULL, i_uniformName);
}

void eae6320::Graphics::Effect::SetUniformHandle(sUniformData i_uniformData, bool i_fromVertexShader)
{
	HRESULT result;
	{
		ID3DXConstantTable* shaderConstantTable = i_fromVertexShader ? m_vertexShaderConstantTable : m_fragmentShaderConstantTable;

		result = shaderConstantTable->SetFloatArray(GetLocalDirect3dDevice(), i_uniformData.uniformHandle, i_uniformData.values, i_uniformData.valueCountToSet);
		assert(SUCCEEDED(result));
	}
}

DWORD eae6320::Graphics::Effect::GetSampler2DID(const char* i_uniformName)
{
	// Textures will always be in fragment shaders for the scope of this project.
	D3DXHANDLE textureHandle = m_fragmentShaderConstantTable->GetConstantByName(NULL, i_uniformName);
	return static_cast<DWORD>(m_fragmentShaderConstantTable->GetSamplerIndex(textureHandle));
}

void eae6320::Graphics::Effect::SetSampler2DID(Texture::tSampler2D i_sampler, Texture::tTextureHandle i_textureHandle, tTextureUnit i_textureUnit)
{
	Graphics::GetLocalDirect3dDevice()->SetTexture(i_sampler, i_textureHandle);
}