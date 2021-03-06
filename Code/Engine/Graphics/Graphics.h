/*
	This file contains the function declarations for graphics
*/

#ifndef EAE6320_GRAPHICS_H
#define EAE6320_GRAPHICS_H

// Header Files
//=============

#include "GameObject.h"
#include "CameraObject.h"
#include "GameSprite.h"
#include "../Windows/WindowsIncludes.h"
#include "DebugMenu.h"
#include "DebugShapes.h"
#include "CollisionDetection.h"

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

		extern DebugSphere* s_debugSphere1;
		extern GameObject* s_debugCylinder1;
		extern GameObject* s_debugCylinder2;

		extern GameObject* s_snowman;
		extern GameObject* s_snowmanClient;
		extern DebugLine* s_snowmanLine;
		extern CollisionDetection* s_collisionDet;

		extern GameObject* s_flag1;
		extern GameObject* s_flag2;
		extern GameObject* s_bullet1;
		extern GameObject* s_bullet2;
		extern DebugMenuBar* s_sprintBar;
		extern DebugMenuScore* s_snowmanScore;
		extern DebugMenuScore* s_snowmanClientScore;
		extern bool s_clientJoined;

		extern GameSprite* s_numbers;

		extern DebugMenuSelection s_activeMenuItem;
		extern DebugMenuCheckBox* s_debugMenuCheckBox;
		extern DebugMenuSlider* s_debugMenuSlider;
		extern DebugMenuButton* s_debugMenuButton;
		extern DebugMenuCheckBox* s_toggleFPSCheckBox;
		extern bool s_debugMenuEnabled;
		extern bool s_isClient;

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

		void DoCOllisions(GameObject* player);

		bool LoadObjects();
		void ReleaseObjects();
	}
}

#endif	// EAE6320_GRAPHICS_H