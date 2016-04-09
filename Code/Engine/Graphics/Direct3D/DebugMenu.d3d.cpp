// Header Files
//=============

#include "../DebugMenu.h"
#include "../Graphics.h"
#include <iostream>
#include <sstream>
#include <iomanip>

// Interface
//==========

// DebugMenuText

void eae6320::Graphics::DebugMenuText::LoadDebugText()
{
	D3DXCreateFont(Graphics::GetLocalDirect3dDevice(), 22, 0, FW_NORMAL, 1, false, DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Arial", &m_font);
}

void eae6320::Graphics::DebugMenuText::DrawDebugText(uint8_t i_color)
{
	m_font->DrawText(NULL, m_message.c_str(), -1, m_textArea, DT_LEFT | DT_NOCLIP, D3DCOLOR_XRGB(i_color, 0, 0));
}

void eae6320::Graphics::DebugMenuText::SetFPS(float i_fps)
{
	std::stringstream ss;
	ss << std::setprecision(std::numeric_limits<float>::digits10 + 1);
	//ss << i_fps;
	ss << s_snowman->m_position.x;
	ss << "*";
	ss << s_snowman->m_position.y;
	ss << "*";
	ss << s_snowman->m_position.z;
	m_message = "FPS = " + ss.str();
}

// DebugMenuCheckBox

void eae6320::Graphics::DebugMenuCheckBox::LoadDebugCheckBox()
{
	m_Text.LoadDebugText();
	m_checkBox.Initialize(GetLocalDirect3dDevice(), "data/checkbox.texture", 40, 20);
}

void eae6320::Graphics::DebugMenuCheckBox::DrawDebugCheckBox(uint8_t i_color)
{
	uint8_t leftCap = 0;
	if (!m_isChecked)
		leftCap = 0;
	else
		leftCap = 20;
	m_checkBox.m_texPortion->left = leftCap;
	m_checkBox.m_texPortion->right = leftCap + 20;

	m_Text.DrawDebugText(i_color);
	m_checkBox.Draw();
}

// DebugMenuSlider

void eae6320::Graphics::DebugMenuSlider::LoadDebugSlider()
{
	m_Text.LoadDebugText();
}

void eae6320::Graphics::DebugMenuSlider::DrawDebugSlider(uint8_t color)
{
	if (sliderCurrentPosition >= 19)
		sliderCurrentPosition = 19;
	if (sliderCurrentPosition <= 0)
		sliderCurrentPosition = 0;

	if (sliderLastPosition >= 19)
		sliderLastPosition = 19;
	if (sliderLastPosition <= 0)
		sliderLastPosition = 0;

	m_sliderMsg.replace(m_sliderMsg.begin() + sliderLastPosition, m_sliderMsg.begin() + sliderLastPosition + 1, "-");
	m_sliderMsg.replace(m_sliderMsg.begin() + sliderCurrentPosition, m_sliderMsg.begin() + sliderCurrentPosition + 1, "*");
	m_Text.m_message = m_textMessage + m_sliderMsg;
	
	sliderLastPosition = sliderCurrentPosition;

	m_Text.DrawDebugText(color);
}

// DebugMenuButton

void eae6320::Graphics::DebugMenuButton::LoadDebugButton()
{
	m_Text.LoadDebugText();
	m_button.Initialize(GetLocalDirect3dDevice(), "data/button.texture", 40, 20);
}

void eae6320::Graphics::DebugMenuButton::DrawDebugButton(uint8_t i_color)
{
	m_Text.DrawDebugText(i_color);
	m_button.Draw();
}