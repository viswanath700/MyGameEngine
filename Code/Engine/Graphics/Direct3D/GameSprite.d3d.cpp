// Header Files
//=============

#include "../GameSprite.h"

// Interface
//==========

eae6320::Graphics::GameSprite::GameSprite()
{
	//Default the sprite to position (0, 0, 0)
	m_position.x = 0;
	m_position.y = 0;
	m_position.z = 0;

	//When color is set to white, what you see is exactly what the image looks like.
	m_color = D3DCOLOR_ARGB(255, 255, 255, 255);

	//We are not initialized yet
	m_initialized = false;

	m_texPortion = NULL;
}

eae6320::Graphics::GameSprite::GameSprite(float x, float y)
{
	//Set the position to the programmer's requested X and Y coordinates.  
	//Default Z to 0
	m_position.x = x;
	m_position.y = y;
	m_position.z = 0;

	//When color is set to white, what you see is exactly what the image looks like.
	m_color = D3DCOLOR_ARGB(255, 255, 255, 255);

	//We are not initialized yet
	m_initialized = false;

	m_texPortion = NULL;
}

bool eae6320::Graphics::GameSprite::Initialize(LPDIRECT3DDEVICE9 device, std::string file, int width, int height)
{
	//Same functionality as D3DXCreateTextureFromFile EXCEPT width and height are manually entered
	if (!SUCCEEDED(D3DXCreateTextureFromFileEx(device, file.c_str(),
		width, height, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT,
		D3DX_DEFAULT, 0, NULL, NULL, &m_texture)))
	{
		std::string s = "There was an issue creating the Texture.  Make sure the requested image is available.  Requested image: " + file;
		MessageBox(NULL, s.c_str(), NULL, NULL);
		return false;
	}

	//Attempt to create the sprite
	if (!SUCCEEDED(D3DXCreateSprite(device, &m_sprite)))
	{
		MessageBox(NULL, "There was an issue creating the Sprite.", NULL, NULL);
		return false;
	}

	m_initialized = true;

	m_texPortion = new RECT();
	m_texPortion->left = 0;
	m_texPortion->top = 0;
	m_texPortion->right = width;
	m_texPortion->bottom = height;

	return true;
}

bool eae6320::Graphics::GameSprite::IsInitialized()
{
	//Are we initialized (have a texture and sprite)
	return m_initialized;
}

void eae6320::Graphics::GameSprite::Draw()
{
	if (m_sprite && m_texture)
	{
		m_sprite->Begin(D3DXSPRITE_ALPHABLEND);

		m_sprite->Draw(m_texture, m_texPortion, NULL, &m_position, m_color);

		m_sprite->End();
	}
}

eae6320::Graphics::GameSprite::~GameSprite()
{
	/*if (m_sprite)
	{
		m_sprite->Release();
		m_sprite = 0;
	}

	if (m_texture)
	{
		m_texture->Release();
		m_texture = 0;
	}*/
}