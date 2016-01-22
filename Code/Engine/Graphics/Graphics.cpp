// Header Files
//=============

#include "Graphics.h"
#include "GameObject.h"
#include "../Math/cMatrix_transformation.h"
#include "../Math/cVector.h"

eae6320::Graphics::CameraObject* eae6320::Graphics::s_camera = NULL;

eae6320::Graphics::GameObject* eae6320::Graphics::s_ceiling_obj = NULL;
eae6320::Graphics::GameObject* eae6320::Graphics::s_cement_obj = NULL;
eae6320::Graphics::GameObject* eae6320::Graphics::s_floor_obj = NULL;
eae6320::Graphics::GameObject* eae6320::Graphics::s_lambert2_obj = NULL;
eae6320::Graphics::GameObject* eae6320::Graphics::s_lambert3_obj = NULL;
eae6320::Graphics::GameObject* eae6320::Graphics::s_metal_obj = NULL;
eae6320::Graphics::GameObject* eae6320::Graphics::s_railing_obj = NULL;
eae6320::Graphics::GameObject* eae6320::Graphics::s_walls_obj = NULL;

bool eae6320::Graphics::LoadObjects()
{
	s_camera = new CameraObject();

	// Loading level
	s_ceiling_obj = new GameObject("data/ceiling.binMesh", "data/ceiling.binMaterial");
	s_cement_obj = new GameObject("data/cement.binMesh", "data/cement.binMaterial");
	s_floor_obj = new GameObject("data/floor.binMesh", "data/floor.binMaterial");
	s_lambert2_obj = new GameObject("data/lambert2.binMesh", "data/lambert2.binMaterial");
	s_lambert3_obj = new GameObject("data/lambert3.binMesh", "data/lambert3.binMaterial");
	s_metal_obj = new GameObject("data/metal.binMesh", "data/metal.binMaterial");
	s_railing_obj = new GameObject("data/railing.binMesh", "data/railing.binMaterial");
	s_walls_obj = new GameObject("data/walls.binMesh", "data/walls.binMaterial");

	// Initialize the level
	if (!s_ceiling_obj->LoadObject() ||
		!s_cement_obj->LoadObject() ||
		!s_floor_obj->LoadObject() ||
		!s_lambert2_obj->LoadObject() ||
		!s_lambert3_obj->LoadObject() ||
		!s_metal_obj->LoadObject() ||
		!s_railing_obj->LoadObject() ||
		!s_walls_obj->LoadObject() )
	{
		return false;
	}

	return true;
}

void eae6320::Graphics::Render()
{
	ClearFrame();
	StartFrame();

	// Drawing Opaque list
	s_ceiling_obj->DrawObject();
	s_cement_obj->DrawObject();
	s_floor_obj->DrawObject();
	s_lambert2_obj->DrawObject();
	s_metal_obj->DrawObject();
	s_railing_obj->DrawObject();
	s_walls_obj->DrawObject();

	// Drawing Transparent list
	//

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