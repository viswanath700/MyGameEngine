/*
This file contains the function declarations for GameSprite
*/

#ifndef GAME_SPRITE_H
#define GAME_SPRITE_H

// Header Files
//=============
#include <d3dx9.h>
#include <string>

// Interface
//==========

namespace eae6320
{
	namespace Graphics
	{
		class GameSprite
		{
		public:
			//Constructors and Destructor
			GameSprite();
			GameSprite(float x, float y);
			~GameSprite();

			//Sprite Functions
			bool Initialize(LPDIRECT3DDEVICE9 device, std::string file, int width, int height);
			bool IsInitialized();
			virtual void Draw();
		private:
			//Pointers
			LPDIRECT3DTEXTURE9 m_texture;
			LPD3DXSPRITE m_sprite;

			//Attributes
			D3DXVECTOR3 m_position;
			D3DCOLOR m_color;
			bool m_initialized;
		};
	}
}

#endif // GAME_SPRITE_H