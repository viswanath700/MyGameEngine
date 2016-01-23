/*
	This file contains the function declarations for graphics
*/

#ifndef EAE6320_GRAPHICS_H
#define EAE6320_GRAPHICS_H

// Header Files
//=============

#include "GameObject.h"
#include "CameraObject.h"
#include "../Windows/WindowsIncludes.h"

#ifdef EAE6320_PLATFORM_D3D
#include <d3d9.h>
#endif // EAE6320_PLATFORM_D3D

// Interface
//==========

namespace eae6320
{
	namespace Graphics
	{
		extern CameraObject* s_camera;

		extern GameObject* s_boxes_obj;
		extern GameObject* s_ceiling_obj;
		extern GameObject* s_floor_obj;
		extern GameObject* s_innerWalls_obj;
		extern GameObject* s_metal_obj;
		extern GameObject* s_outerWalls_obj;
		extern GameObject* s_railing_obj;

		bool Initialize( const HWND i_renderingWindow );
		void Render();
		bool ShutDown();

#ifdef EAE6320_PLATFORM_D3D
		IDirect3DDevice9* GetLocalDirect3dDevice();
#endif // EAE6320_PLATFORM_D3D

		bool ClearFrame();
		bool StartFrame();
		bool EndFrame();
		bool ShowFrame();

		bool LoadObjects();
		void ReleaseObjects();
	}
}

#endif	// EAE6320_GRAPHICS_H