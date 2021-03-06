// Header Files
//=============

#include "Graphics.h"
#include "GameObject.h"
#include "../Math/cMatrix_transformation.h"
#include "../Math/cVector.h"
#include "../Time/Time.h"
#include "GameSprite.h"

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

eae6320::Graphics::GameObject* eae6320::Graphics::s_snowman = NULL;
eae6320::Graphics::GameObject* eae6320::Graphics::s_snowmanClient = NULL;
eae6320::Graphics::DebugLine* eae6320::Graphics::s_snowmanLine = NULL;
eae6320::Graphics::CollisionDetection* eae6320::Graphics::s_collisionDet = NULL;

eae6320::Graphics::GameObject* eae6320::Graphics::s_flag1 = NULL;
eae6320::Graphics::GameObject* eae6320::Graphics::s_flag2 = NULL;
eae6320::Graphics::GameObject* eae6320::Graphics::s_bullet1 = NULL;
eae6320::Graphics::GameObject* eae6320::Graphics::s_bullet2= NULL;
eae6320::Graphics::DebugMenuBar* eae6320::Graphics::s_sprintBar = NULL;
eae6320::Graphics::DebugMenuScore* eae6320::Graphics::s_snowmanScore = NULL;
eae6320::Graphics::DebugMenuScore* eae6320::Graphics::s_snowmanClientScore = NULL;
bool eae6320::Graphics::s_clientJoined = false;

eae6320::Graphics::DebugLine s_debugLine1;
eae6320::Graphics::DebugLine s_debugLine2;
eae6320::Graphics::DebugBox s_debugBox1;
eae6320::Graphics::DebugBox s_debugBox2;
eae6320::Graphics::DebugSphere* eae6320::Graphics::s_debugSphere1 = NULL;
eae6320::Graphics::DebugSphere s_debugSphere2;

eae6320::Graphics::GameSprite s_logo;
eae6320::Graphics::GameSprite* eae6320::Graphics::s_numbers = NULL;

eae6320::Graphics::DebugMenuSelection eae6320::Graphics::s_activeMenuItem;
eae6320::Graphics::DebugMenuText s_debugMenuTextFPS;
eae6320::Graphics::DebugMenuCheckBox* eae6320::Graphics::s_debugMenuCheckBox = NULL;
eae6320::Graphics::DebugMenuSlider* eae6320::Graphics::s_debugMenuSlider = NULL;
eae6320::Graphics::DebugMenuButton* eae6320::Graphics::s_debugMenuButton = NULL;
eae6320::Graphics::DebugMenuCheckBox* eae6320::Graphics::s_toggleFPSCheckBox = NULL;

bool eae6320::Graphics::s_debugMenuEnabled = false;
bool eae6320::Graphics::s_isClient = false;

eae6320::Math::cVector s_playerPrevPos;
eae6320::Math::cVector s_camPrevPos;

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

	// Debug Shapes
	s_debugLine1 = eae6320::Graphics::DebugLine(Math::cVector(100.0f, 0.0f, 0.0f), Math::cVector(75.0f, -50.0f, -50.0f), Math::cVector(0.0f, 0.4f, 0.2f));
	s_debugLine2 = eae6320::Graphics::DebugLine(Math::cVector(-100.0f, 0.0f, -70.0f), Math::cVector(-50.0f, -50.0f, -50.0f), Math::cVector(1.0f, 0.0f, 0.0f));
	s_debugBox1 = eae6320::Graphics::DebugBox(Math::cVector(30.0f, -20.0f, -40.0f), 20.0f, Math::cVector(1.0f, 0.0f, 1.0f));
	s_debugBox2 = eae6320::Graphics::DebugBox(Math::cVector(-30.0f, 20.0f, -40.0f), 15.0f, Math::cVector(0.0f, 0.0f, 0.3f));
	s_debugSphere1 = new DebugSphere(Math::cVector(-50.0f, 0.0f, -150.0f), 20.0f, 20, 20, Math::cVector(0.2f, 0.4f, 0.0f));
	s_debugSphere2 = eae6320::Graphics::DebugSphere(Math::cVector(-50.0f, 0.0f, -250.0f), 30.0f, 20, 20, Math::cVector(0.0f, 1.0f, 1.0f));
	s_debugCylinder1 = new GameObject("data/cylinder1.binMesh", "data/cylinder1.binMaterial");
	s_debugCylinder2 = new GameObject("data/cylinder2.binMesh", "data/cylinder2.binMaterial");

	// Game Sprites
	s_logo = GameSprite(10, 10);
	s_numbers = new GameSprite(650, 100);

	// Third person snowman
	s_snowman = new GameObject("data/snowman.binMesh", "data/snowman.binMaterial");
	s_snowmanClient = new GameObject("data/snowman.binMesh", "data/snowmanClient.binMaterial");
	s_snowmanLine = new eae6320::Graphics::DebugLine(Math::cVector(100.0f, 0.0f, 0.0f), Math::cVector(75.0f, -50.0f, -50.0f), Math::cVector(0.0f, 0.0f, 1.0f));
	s_collisionDet = new eae6320::Graphics::CollisionDetection();

	// Capture the Flag
	s_flag1 = new GameObject("data/flag1.binMesh", "data/flag1.binMaterial");
	s_flag2 = new GameObject("data/flag2.binMesh", "data/flag2.binMaterial");
	s_bullet1 = new GameObject("data/bullet.binMesh", "data/bullet1.binMaterial");
	s_bullet2 = new GameObject("data/bullet.binMesh", "data/bullet2.binMaterial");
	s_sprintBar = new DebugMenuBar("Sprint  ", 300, 20, 200, 50);
	s_snowmanScore = new DebugMenuScore("Player 1 Score = ", 300, 50, 200, 50);
	s_snowmanClientScore = new DebugMenuScore("Player 2 Score = ", 300, 80, 200, 50);

#ifdef _DEBUG
	// Debug Menu Stuff
	s_debugMenuTextFPS = eae6320::Graphics::DebugMenuText("FPS = ", 20, 20, 150, 50);
	s_debugMenuCheckBox = new eae6320::Graphics::DebugMenuCheckBox("Enable Debug Sphere ", 20, 50, 200, 50);
	s_debugMenuSlider = new eae6320::Graphics::DebugMenuSlider("Volume ", 20, 80, 200, 50);
	s_debugMenuButton = new eae6320::Graphics::DebugMenuButton("Reset Radius of Debug Sphere ", 20, 110, 275, 50);
	s_toggleFPSCheckBox = new eae6320::Graphics::DebugMenuCheckBox("Enable Fly Camera ", 20, 140, 200, 50);
#endif

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

	// Loading Debug Shapes
	s_debugLine1.LoadDebugLine();
	s_debugLine2.LoadDebugLine();
	s_debugBox1.LoadDebugBox();
	s_debugBox2.LoadDebugBox();
	s_debugSphere1->LoadDebugSphere();
	s_debugSphere2.LoadDebugSphere();
	s_debugCylinder1->LoadObject();
	s_debugCylinder2->LoadObject();

	// Loading Game Sprites
	s_logo.Initialize(GetLocalDirect3dDevice(), "data/logo.texture", 256, 256);
	s_numbers->Initialize(GetLocalDirect3dDevice(), "data/numbers.texture", 512, 64);

#ifdef _DEBUG
	// Loading DebugMebu Items
	s_activeMenuItem = DebugMenuSelection::Text;
	s_debugMenuTextFPS.LoadDebugText();
	s_debugMenuCheckBox->LoadDebugCheckBox();
	s_debugMenuSlider->LoadDebugSlider();
	s_debugMenuButton->LoadDebugButton();
	s_toggleFPSCheckBox->LoadDebugCheckBox();
#endif

	// Enabling third person camera
	s_toggleFPSCheckBox->m_isChecked = false;

	// Loading Third person Snowman
	s_snowmanLine->LoadDebugLine();
	s_snowman->LoadObject();
	s_snowman->m_position.x -= 50;
	s_snowman->m_position.y -= 220;
	s_snowman->m_position.z -= 300;

	s_snowmanClient->LoadObject();
	s_snowmanClient->m_position.x -= 50;
	s_snowmanClient->m_position.y -= 220;
	s_snowmanClient->m_position.z -= 0;

	s_collisionDet->LoadCollisionData("data/CollisionData.binmesh");

	// Capture the Flag
	s_flag1->LoadObject();
	s_flag2->LoadObject();
	s_bullet1->LoadObject();
	s_bullet2->LoadObject();
	s_sprintBar->LoadDebugBar();
	s_snowmanScore->LoadDebugText();
	s_snowmanClientScore->LoadDebugText();

	s_flag1->m_position = s_flag1->m_defaultPosition = Math::cVector(1487, 57, -1452);
	s_flag2->m_position = s_flag2->m_defaultPosition = Math::cVector(-1487, 57, 1452);
	s_bullet1->m_isActive = false;
	s_bullet2->m_isActive = false;
	s_snowmanScore->m_score = 0;
	s_snowmanClientScore->m_score = 0;

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
	s_snowmanLine->DrawLine();
	s_debugBox1.DrawBox();
	s_debugBox2.DrawBox();

	if (s_debugMenuCheckBox->m_isChecked)
	{
		s_debugSphere1->DrawSphere();
		//s_debugSphere2.DrawSphere();
	}

	Graphics::GetLocalDirect3dDevice()->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	s_debugCylinder1->DrawObject();
	s_debugCylinder2->DrawObject();
	Graphics::GetLocalDirect3dDevice()->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
#endif

	// Drawing Game Sprites
	//s_logo.Draw();
	s_numbers->Draw();

	// Drawing third person snowman
	s_snowman->DrawObject();
	s_snowmanClient->DrawObject();

	// Draw Capture the Flag Stuff
	s_flag1->DrawObject();
	s_flag2->DrawObject();
	if(s_bullet1->m_isActive)
		s_bullet1->DrawObject();
	if (s_bullet2->m_isActive)
		s_bullet2->DrawObject();
	s_sprintBar->DrawDebugBar(150);
	s_snowmanScore->DrawDebugText(100);
	if(s_clientJoined)
		s_snowmanClientScore->DrawDebugText(100);

	// Drawing Debug Menu Items
#ifdef _DEBUG
	if (s_debugMenuEnabled)
	{
		float fpsCount = 1 / Time::GetSecondsElapsedThisFrame();
		s_debugMenuTextFPS.SetFPS(fpsCount);

		switch (s_activeMenuItem)
		{
		case DebugMenuSelection::Text:
			s_debugMenuTextFPS.DrawDebugText(255);
			s_debugMenuCheckBox->DrawDebugCheckBox(0);
			s_debugMenuSlider->DrawDebugSlider(0);
			s_debugMenuButton->DrawDebugButton(0);
			s_toggleFPSCheckBox->DrawDebugCheckBox(0);
			break;

		case DebugMenuSelection::CheckBox:
			s_debugMenuTextFPS.DrawDebugText(0);
			s_debugMenuCheckBox->DrawDebugCheckBox(255);
			s_debugMenuSlider->DrawDebugSlider(0);
			s_debugMenuButton->DrawDebugButton(0);
			s_toggleFPSCheckBox->DrawDebugCheckBox(0);
			break;

		case DebugMenuSelection::Slider:
			s_debugMenuTextFPS.DrawDebugText(0);
			s_debugMenuCheckBox->DrawDebugCheckBox(0);
			s_debugMenuSlider->DrawDebugSlider(255);
			s_debugMenuButton->DrawDebugButton(0);
			s_toggleFPSCheckBox->DrawDebugCheckBox(0);
			break;

		case DebugMenuSelection::Button:
			s_debugMenuTextFPS.DrawDebugText(0);
			s_debugMenuCheckBox->DrawDebugCheckBox(0);
			s_debugMenuSlider->DrawDebugSlider(0);
			s_debugMenuButton->DrawDebugButton(255);
			s_toggleFPSCheckBox->DrawDebugCheckBox(0);
			break;

		case DebugMenuSelection::ToggleCam:
			s_debugMenuTextFPS.DrawDebugText(0);
			s_debugMenuCheckBox->DrawDebugCheckBox(0);
			s_debugMenuSlider->DrawDebugSlider(0);
			s_debugMenuButton->DrawDebugButton(0);
			s_toggleFPSCheckBox->DrawDebugCheckBox(255);
			break;

		default:
			break;
		}
	}
#endif

	EndFrame();
	ShowFrame();
}

void eae6320::Graphics::DoCOllisions(GameObject* player)
{
	// COllision Detection 
	player->m_velocity = player->m_position - s_playerPrevPos;
	player->m_position = s_collisionDet->CollideWithWorld(player->m_position, player->m_velocity, 50, false);
	player->m_velocity = Math::cVector(0, -9.8f, 0) * 5;
	player->m_position = s_collisionDet->CollideWithWorld(player->m_position, player->m_velocity, 20, false);
	s_playerPrevPos = player->m_position;

	Math::cVector playerDir = player->m_position - s_camera->m_position;
	Math::cMatrix_transformation mat = Math::cMatrix_transformation::cMatrix_transformation(player->m_orientation, player->m_position);
	Math::cVector offset = Math::cMatrix_transformation::matrixMulVector(mat, Math::cVector(0, 120, 200));
	Math::cVector rayOrigin = offset;
	s_collisionDet->CollideWithWorld(rayOrigin, playerDir, playerDir.GetLength(), true);

}

void eae6320::Graphics::ReleaseObjects()
{
	//s_cone_obj->ReleaseGameObject();
	//s_sphere_obj->ReleaseGameObject();
	//s_helix_obj->ReleaseGameObject();
	//s_floor_obj->ReleaseGameObject();
}