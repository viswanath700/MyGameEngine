// Header Files
//=============

#include "Graphics.h"
#include "GameObject.h"
#include "../Math/cMatrix_transformation.h"
#include "../Math/cVector.h"
#include "DebugShapes.h"

eae6320::Graphics::CameraObject* eae6320::Graphics::s_camera = NULL;

eae6320::Graphics::GameObject* eae6320::Graphics::s_boxes_obj = NULL;
eae6320::Graphics::GameObject* eae6320::Graphics::s_ceiling_obj = NULL;
eae6320::Graphics::GameObject* eae6320::Graphics::s_floor_obj = NULL;
eae6320::Graphics::GameObject* eae6320::Graphics::s_innerWalls_obj = NULL;
eae6320::Graphics::GameObject* eae6320::Graphics::s_metal_obj = NULL;
eae6320::Graphics::GameObject* eae6320::Graphics::s_outerWalls_obj = NULL;
eae6320::Graphics::GameObject* eae6320::Graphics::s_railing_obj = NULL;

eae6320::Graphics::DebugLine s_debugLine1;
eae6320::Graphics::DebugBox s_debugBox1;
eae6320::Graphics::DebugSphere s_debugSphere1;

bool eae6320::Graphics::LoadObjects()
{
	s_camera = new CameraObject();

	// Loading level
	s_boxes_obj = new GameObject("data/boxes.binMesh", "data/boxes.binMaterial");
	s_ceiling_obj = new GameObject("data/ceiling.binMesh", "data/ceiling.binMaterial");
	s_floor_obj = new GameObject("data/floor.binMesh", "data/floor.binMaterial");
	s_innerWalls_obj = new GameObject("data/innerWalls.binMesh", "data/innerWalls.binMaterial");
	s_metal_obj = new GameObject("data/metal.binMesh", "data/metal.binMaterial");
	s_outerWalls_obj = new GameObject("data/outerWalls.binMesh", "data/outerWalls.binMaterial");
	s_railing_obj = new GameObject("data/railing.binMesh", "data/railing.binMaterial");

	s_debugLine1 = eae6320::Graphics::DebugLine(Math::cVector(0.0f, 0.0f, 0.0f), Math::cVector(50.0f, 50.0f, 50.0f), Math::cVector(1.0f, 0.0f, 0.0f));
	s_debugBox1 = eae6320::Graphics::DebugBox(Math::cVector(30.0f, -20.0f, -40.0f), 20.0f, Math::cVector(0.0f, 1.0f, 0.0f));
	s_debugSphere1 = eae6320::Graphics::DebugSphere(Math::cVector(0.0f, 0.0f, 0.0f), 30.0f, 20, 20, Math::cVector(0.0f, 0.0f, 1.0f));

	// Initialize the level
	if (!s_boxes_obj->LoadObject() ||
		!s_ceiling_obj->LoadObject() ||
		!s_floor_obj->LoadObject() ||
		!s_innerWalls_obj->LoadObject() ||
		!s_metal_obj->LoadObject() ||
		!s_outerWalls_obj->LoadObject() ||
		!s_railing_obj->LoadObject() )
	{
		return false;
	}

	s_debugLine1.LoadDebugLine();
	s_debugBox1.LoadDebugBox();
	s_debugSphere1.LoadDebugSphere();

	return true;
}

void eae6320::Graphics::Render()
{
	ClearFrame();
	StartFrame();

	// Drawing Opaque list
	s_boxes_obj->DrawObject();
	s_ceiling_obj->DrawObject();
	s_floor_obj->DrawObject();
	s_innerWalls_obj->DrawObject();
	s_metal_obj->DrawObject();
	s_outerWalls_obj->DrawObject();
	s_railing_obj->DrawObject();

	// Drawing Transparent list
	//

	// Drawing Debug Shapes
	s_debugLine1.DrawLine();
	s_debugBox1.DrawBox();
	s_debugSphere1.DrawSphere();

	EndFrame();
	ShowFrame();
}

void eae6320::Graphics::ReleaseObjects()
{
	//s_cone_obj->ReleaseGameObject();
	//s_sphere_obj->ReleaseGameObject();
	//s_helix_obj->ReleaseGameObject();
	//s_floor_obj->ReleaseGameObject();
}