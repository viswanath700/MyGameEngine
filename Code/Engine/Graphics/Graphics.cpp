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

eae6320::Graphics::GameObject* eae6320::Graphics::s_debugCylinder1 = NULL;
eae6320::Graphics::GameObject* eae6320::Graphics::s_debugCylinder2 = NULL;

eae6320::Graphics::DebugLine s_debugLine1;
eae6320::Graphics::DebugLine s_debugLine2;
eae6320::Graphics::DebugBox s_debugBox1;
eae6320::Graphics::DebugBox s_debugBox2;
eae6320::Graphics::DebugSphere s_debugSphere1;
eae6320::Graphics::DebugSphere s_debugSphere2;

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

	s_debugLine1 = eae6320::Graphics::DebugLine(Math::cVector(100.0f, 0.0f, 0.0f), Math::cVector(75.0f, -50.0f, -50.0f), Math::cVector(0.0f, 0.4f, 0.2f));
	s_debugLine2 = eae6320::Graphics::DebugLine(Math::cVector(-100.0f, 0.0f, -70.0f), Math::cVector(-50.0f, -50.0f, -50.0f), Math::cVector(1.0f, 0.0f, 0.0f));
	s_debugBox1 = eae6320::Graphics::DebugBox(Math::cVector(30.0f, -20.0f, -40.0f), 20.0f, Math::cVector(1.0f, 0.0f, 1.0f));
	s_debugBox2 = eae6320::Graphics::DebugBox(Math::cVector(-30.0f, 20.0f, -40.0f), 15.0f, Math::cVector(0.0f, 0.0f, 0.3f));
	s_debugSphere1 = eae6320::Graphics::DebugSphere(Math::cVector(-50.0f, 0.0f, -150.0f), 20.0f, 20, 20, Math::cVector(0.2f, 0.4f, 0.0f));
	s_debugSphere2 = eae6320::Graphics::DebugSphere(Math::cVector(-50.0f, 0.0f, -250.0f), 30.0f, 20, 20, Math::cVector(0.0f, 1.0f, 1.0f));
	s_debugCylinder1 = new GameObject("data/cylinder1.binMesh", "data/cylinder1.binMaterial");
	s_debugCylinder2 = new GameObject("data/cylinder2.binMesh", "data/cylinder2.binMaterial");

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
	s_debugLine2.LoadDebugLine();
	s_debugBox1.LoadDebugBox();
	s_debugBox2.LoadDebugBox();
	s_debugSphere1.LoadDebugSphere();
	s_debugSphere2.LoadDebugSphere();
	s_debugCylinder1->LoadObject();
	s_debugCylinder2->LoadObject();

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
#ifdef _DEBUG
	s_debugLine1.DrawLine();
	s_debugLine2.DrawLine();
	s_debugBox1.DrawBox();
	s_debugBox2.DrawBox();
	s_debugSphere1.DrawSphere();
	s_debugSphere2.DrawSphere();

	Graphics::GetLocalDirect3dDevice()->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	s_debugCylinder1->DrawObject();
	s_debugCylinder2->DrawObject();
	Graphics::GetLocalDirect3dDevice()->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
#endif

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