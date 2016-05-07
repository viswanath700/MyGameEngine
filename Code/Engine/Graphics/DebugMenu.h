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
		enum DebugMenuSelection
		{
			Text, CheckBox, Slider, Button, ToggleCam
		};

		class DebugMenuText
		{
		public:
			std::string m_message;
		private:
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
			void DrawDebugText(uint8_t color);
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
			void DrawDebugCheckBox(uint8_t i_color);
		};

		class DebugMenuSlider
		{
		public:
			uint8_t value;
			DebugMenuText m_Text;
			std::string m_textMessage;
			std::string m_sliderMsg;
			int sliderCurrentPosition;
			int sliderLastPosition;

		public:
			DebugMenuSlider() {};
			DebugMenuSlider(std::string i_message, float i_posX, float i_posY, float i_width, float i_height)
			{
				m_textMessage = i_message;
				m_sliderMsg = "*-------------------";
				m_Text = DebugMenuText(i_message, i_posX, i_posY, i_width, i_height);
			}

			void LoadDebugSlider();
			void DrawDebugSlider(uint8_t color);
		};

		class DebugMenuButton
		{
			GameSprite m_button;
			DebugMenuText m_Text;

		public:
			DebugMenuButton() {};
			DebugMenuButton(std::string i_message, float i_posX, float i_posY, float i_width, float i_height)
			{
				m_Text = DebugMenuText(i_message, i_posX, i_posY, i_width, i_height);
				m_button = GameSprite(i_posX + i_width, i_posY);
			}

			void LoadDebugButton();
			void DrawDebugButton(uint8_t i_color);
		};

		class DebugMenuBar
		{
		public:
			uint8_t value;
			DebugMenuText m_Text;
			std::string m_textMessage;
			std::string m_sliderMsg;
			float barPosition;

		public:
			DebugMenuBar() {};
			DebugMenuBar(std::string i_message, float i_posX, float i_posY, float i_width, float i_height)
			{
				barPosition = 20;
				m_textMessage = i_message;
				m_sliderMsg = "####################";
				m_Text = DebugMenuText(i_message, i_posX, i_posY, i_width, i_height);
			}

			void LoadDebugBar();
			void DrawDebugBar(uint8_t color);
		};

		class DebugMenuScore
		{
		public:
			std::string m_message;
			uint8_t m_score;
		private:
			LPD3DXFONT m_font;
			LPRECT m_textArea;

		public:
			DebugMenuScore() {};
			DebugMenuScore(std::string i_message, float i_posX, float i_posY, float i_width, float i_height) :
				m_message(i_message),
				m_font(NULL)
			{
				m_score = 0;

				m_textArea = new RECT();
				m_textArea->left = i_posX;
				m_textArea->top = i_posY;
				m_textArea->right = i_posX + i_width;
				m_textArea->bottom = i_posY + i_height;
			};

			void LoadDebugText();
			void DrawDebugText(uint8_t color);
		};
	}
}

#endif	// DEBUG_MENU_H