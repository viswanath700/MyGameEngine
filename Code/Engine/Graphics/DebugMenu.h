/*
This file contains the function declarations for DebugMenu
*/

#ifndef DEBUG_MENU_H
#define DEBUG_MENU_H

// Header Files
//=============
#include <d3dx9.h>
#include <string>
#include "GameSprite.h"

// Interface
//==========

namespace eae6320
{
	namespace Graphics
	{
		class DebugMenuText
		{
		private:
			std::string m_message;
			LPD3DXFONT m_font;
			LPRECT m_textArea;

		public:
			DebugMenuText() {};
			DebugMenuText(std::string i_message, float i_posX, float i_posY, float i_width, float i_height) :
				m_message(i_message),
				m_font(NULL)
			{
				m_textArea = new RECT();
				m_textArea->left = i_posX;
				m_textArea->top = i_posY;
				m_textArea->right = i_posX + i_width;
				m_textArea->bottom = i_posY + i_height;
			};

			void LoadDebugText();
			void DrawDebugText();
			void SetFPS(float i_fps);
		};

		class DebugMenuCheckBox
		{
		public:
			bool m_isChecked;
			GameSprite m_checkBox;

		private:
			DebugMenuText m_Text;

		public:
			DebugMenuCheckBox() {};
			DebugMenuCheckBox(std::string i_message, float i_posX, float i_posY, float i_width, float i_height)
			{
				m_isChecked = true;
				m_Text = DebugMenuText(i_message, i_posX, i_posY, i_width, i_height);
				m_checkBox = GameSprite(i_posX + i_width, i_posY);
			}

			void LoadDebugCheckBox();
			void DrawDebugCheckBox();
		};
	}
}

#endif	// DEBUG_MENU_H