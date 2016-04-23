/*
This file contains the function declarations for Game
*/

#ifndef EAE6320_GAME_H
#define EAE6320_GAME_H

// Header Files
//=============

#include "../../Engine/Windows/WindowsIncludes.h"

// Interface
//==========

namespace Game
{
	bool Initialize(const HWND i_renderingWindow, bool serverState);
	void Run();
	bool ShutDown();
}

#endif	// EAE6320_GAME_H