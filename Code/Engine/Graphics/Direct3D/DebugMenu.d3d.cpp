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

void eae6320::Graphics::DebugMenuText::DrawDebugText()
{
	m_font->DrawText(NULL, m_message.c_str(), -1, m_textArea, DT_LEFT | DT_NOCLIP, D3DCOLOR_XRGB(0, 0, 0));
}

void eae6320::Graphics::DebugMenuText::SetFPS(float i_fps)
{
	std::stringstream ss;
	ss << std::setprecision(std::numeric_limits<float>::digits10 + 1);
	ss << i_fps;
	m_message = "FPS = " + ss.str();
}

// DebugMenuCheckBox

void eae6320::Graphics::DebugMenuCheckBox::LoadDebugCheckBox()
{
	m_Text.LoadDebugText();
	m_checkBox.Initialize(GetLocalDirect3dDevice(), "data/checkbox.texture", 20, 20);
}

void eae6320::Graphics::DebugMenuCheckBox::DrawDebugCheckBox()
{
	m_Text.DrawDebugText();
	m_checkBox.Draw();
}