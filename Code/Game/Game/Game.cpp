// Header Files
//=============

#include "Game.h"
#include "../../Engine/Time/Time.h"
#include "../../Engine/Math/cVector.h"
#include "../../Engine/Math/cQuaternion.h"
#include "../../Engine/Math/cMatrix_transformation.h"
#include "../../Engine/UserInput/UserInput.h"
#include "../../Engine/Graphics/Graphics.h"
#include "../../Engine/Graphics/DebugShapes.h"
#include <math.h>

namespace Game
{
	uint8_t pressedNum = 0;
	bool tildePressed = false;
	uint8_t debugMenuSelection = 0;
	bool upPressed = false;
	bool downPressed = false;
	bool rightPressed = false;
	bool leftPressed = false;

	// Helper to update renderable position
	bool UpdateEntities_vector()
	{
		bool wereThereErrors = false;

		eae6320::Math::cVector offset(0.0f, 0.0f, 0.0f);
		eae6320::Math::cVector rotationOffset(0.0f, 0.0f, 0.0f);
		eae6320::Math::cVector thirdPersonCamOffset(0.0f, 0.0f, 0.0f);
		{
			{
				// Get the direction
				if (eae6320::UserInput::IsKeyPressed('A'))
				{
					offset.x -= 1.0f;
				}
				if (eae6320::UserInput::IsKeyPressed('D'))
				{
					offset.x += 1.0f;
				}
				if (eae6320::UserInput::IsKeyPressed('W'))
				{
					offset.z -= 1.0f;
				}
				if (eae6320::UserInput::IsKeyPressed('S'))
				{
					offset.z += 1.0f;
				}
				if (eae6320::UserInput::IsKeyPressed(VK_UP))
				{
					offset.y += 1.0f;
				}
				if (eae6320::UserInput::IsKeyPressed(VK_DOWN))
				{
					offset.y -= 1.0f;
				}

				// Get rotation
				if (eae6320::UserInput::IsKeyPressed(VK_LEFT))
				{
					rotationOffset.y -= 0.3f;
				}
				if (eae6320::UserInput::IsKeyPressed(VK_RIGHT))
				{
					rotationOffset.y += 0.3f;
				}

				if (eae6320::UserInput::IsKeyPressed('Z'))
				{
					thirdPersonCamOffset.x -= 1.0f;
				}
				if (eae6320::UserInput::IsKeyPressed('X'))
				{
					thirdPersonCamOffset.x += 1.0f;
				}
			}
			// Get the speed
			const float unitsPerSecond = 300.0f;	// This is arbitrary
			const float unitsToMove = unitsPerSecond * eae6320::Time::GetSecondsElapsedThisFrame();	// This makes the speed frame-rate-independent
																									// Normalize the offset
			offset *= unitsToMove;
			rotationOffset *= unitsToMove / 100.0f;
		}

		if (eae6320::Graphics::s_toggleFPSCheckBox->m_isChecked)
		{
			// Setting camera rotation once for x axis and once for y axis
			eae6320::Graphics::s_camera->UpdateOrientation(rotationOffset);

			// The following line assumes there is some "entity" for the rectangle that the game code controls
			// that encapsulates a mesh, an effect, and a position offset.
			// You don't have to do it this way for your assignment!
			// You just need a way to update the position offset associated with the colorful rectangle.
			eae6320::Graphics::s_camera->UpdatePosition(offset);
		}
		else
		{
			eae6320::Math::cMatrix_transformation localToWorldTransformCamera = eae6320::Math::cMatrix_transformation(
				eae6320::Graphics::s_camera->m_orientation, eae6320::Graphics::s_camera->m_position);
			eae6320::Graphics::s_camera->m_position = eae6320::Math::cMatrix_transformation::matrixMulVector(localToWorldTransformCamera, eae6320::Math::cVector(0, 0, -300));
			eae6320::Graphics::s_camera->UpdateOrientation(rotationOffset); 
			localToWorldTransformCamera = eae6320::Math::cMatrix_transformation(eae6320::Graphics::s_camera->m_orientation, eae6320::Graphics::s_camera->m_position);
			eae6320::Graphics::s_camera->m_position = eae6320::Math::cMatrix_transformation::matrixMulVector(localToWorldTransformCamera, eae6320::Math::cVector(0, 0, 300));

			eae6320::Math::cVector oldSnowmanPos = eae6320::Graphics::s_snowman->m_position;
			offset.y = 0.0f;

			eae6320::Graphics::s_camera->UpdatePosition(thirdPersonCamOffset);

			//eae6320::Math::cMatrix_transformation i_localToWorldTransformCamera = eae6320::Math::cMatrix_transformation(
			//	eae6320::Graphics::s_camera->m_orientation, eae6320::Graphics::s_camera->m_position);
			//eae6320::Math::cVector cameraPos = eae6320::Math::cMatrix_transformation::matrixMulVector(i_localToWorldTransformCamera, eae6320::Math::cVector());

			eae6320::Math::cMatrix_transformation i_localToWorldTransformSnowman = eae6320::Math::cMatrix_transformation(
				eae6320::Graphics::s_camera->m_orientation, eae6320::Graphics::s_snowman->m_position);
			eae6320::Math::cVector newSnowmanPos = eae6320::Math::cMatrix_transformation::matrixMulVector(i_localToWorldTransformSnowman, offset);

			eae6320::Graphics::s_snowman->m_position.x = newSnowmanPos.x;
			eae6320::Graphics::s_snowman->m_position.z = newSnowmanPos.z;

			eae6320::Math::cVector deltaVector = eae6320::Math::cVector(newSnowmanPos.x - eae6320::Graphics::s_camera->m_position.x, 0.0f,
				newSnowmanPos.z - (eae6320::Graphics::s_camera->m_position.z - 300));
			eae6320::Math::cMatrix_transformation cameraRotationTransform = eae6320::Math::cMatrix_transformation(
				eae6320::Graphics::s_camera->m_orientation, eae6320::Math::cVector());
			cameraRotationTransform.Transpose();
			eae6320::Math::cVector newDeltaVector = eae6320::Math::cMatrix_transformation::matrixMulVector(cameraRotationTransform, deltaVector);

			float deltaX = abs(newDeltaVector.x);
			float deltaZ = abs(newDeltaVector.z); 

			if (deltaX > 100 || deltaZ > 100)
				eae6320::Graphics::s_camera->UpdatePosition(offset);



			// Updating debug line for the player
			oldSnowmanPos.y += 30;
			newSnowmanPos.y = oldSnowmanPos.y;
			if (!(newSnowmanPos == oldSnowmanPos))
			{
				eae6320::Math::cVector directionVector = (newSnowmanPos - oldSnowmanPos);
				directionVector.Normalize();
				eae6320::Graphics::s_snowmanLine->m_startPoint = newSnowmanPos;
				eae6320::Graphics::s_snowmanLine->m_endPoint = newSnowmanPos + (directionVector * 50);
				eae6320::Graphics::s_snowmanLine->LoadDebugLine();
			}
		}
		
		return !wereThereErrors;
	}

	// Helper to detect shooting based on user input
	bool UpdateSprite()
	{
		bool wereThereErrors = false;

		if (eae6320::UserInput::IsKeyPressed('0') || eae6320::UserInput::IsKeyPressed(VK_NUMPAD0))
		{
			pressedNum = 0;
		}
		if (eae6320::UserInput::IsKeyPressed('1') || eae6320::UserInput::IsKeyPressed(VK_NUMPAD1))
		{
			pressedNum = 1;
		}
		if (eae6320::UserInput::IsKeyPressed('2') || eae6320::UserInput::IsKeyPressed(VK_NUMPAD2))
		{
			pressedNum = 2;
		}
		if (eae6320::UserInput::IsKeyPressed('3') || eae6320::UserInput::IsKeyPressed(VK_NUMPAD3))
		{
			pressedNum = 3;
		}
		if (eae6320::UserInput::IsKeyPressed('4') || eae6320::UserInput::IsKeyPressed(VK_NUMPAD4))
		{
			pressedNum = 4;
		}
		if (eae6320::UserInput::IsKeyPressed('5') || eae6320::UserInput::IsKeyPressed(VK_NUMPAD5))
		{
			pressedNum = 5;
		}
		if (eae6320::UserInput::IsKeyPressed('6') || eae6320::UserInput::IsKeyPressed(VK_NUMPAD6))
		{
			pressedNum = 6;
		}
		if (eae6320::UserInput::IsKeyPressed('7') || eae6320::UserInput::IsKeyPressed(VK_NUMPAD7))
		{
			pressedNum = 7;
		}
		if (eae6320::UserInput::IsKeyPressed('8') || eae6320::UserInput::IsKeyPressed(VK_NUMPAD8))
		{
			pressedNum = 8;
		}
		if (eae6320::UserInput::IsKeyPressed('9') || eae6320::UserInput::IsKeyPressed(VK_NUMPAD9))
		{
			pressedNum = 9;
		}

		float eachBlockSize = 512 / 10;
		eae6320::Graphics::s_numbers->m_texPortion->left = eachBlockSize * pressedNum;
		eae6320::Graphics::s_numbers->m_texPortion->right = eachBlockSize * (pressedNum + 1);

		return !wereThereErrors;
	}

	// Enable DebugMenu
	void EnableDebugMenu()
	{
		if (eae6320::UserInput::IsKeyPressed(VK_OEM_3))
		{
			if (!tildePressed)
			{
				if (eae6320::Graphics::s_debugMenuEnabled == true)
					eae6320::Graphics::s_debugMenuEnabled = false;
				else
					eae6320::Graphics::s_debugMenuEnabled = true;
			}

			tildePressed = true;
		}
		else
			tildePressed = false;
	}

	void UpdateSelectedMenuItem(bool isRight)
	{
		switch (debugMenuSelection)
		{
		case 0:
			// Do Nothing
			break;

		case 1:
			if (eae6320::Graphics::s_debugMenuCheckBox->m_isChecked == true)
				eae6320::Graphics::s_debugMenuCheckBox->m_isChecked = false;
			else
				eae6320::Graphics::s_debugMenuCheckBox->m_isChecked = true;
			break;

		case 2:
		{
			float radiusOffset = 2;
			if (isRight)
			{
				if (!(eae6320::Graphics::s_debugMenuSlider->sliderCurrentPosition == 19))
				{
					eae6320::Graphics::s_debugSphere1->m_radius += radiusOffset;
					eae6320::Graphics::s_debugMenuSlider->sliderCurrentPosition++;
					if (eae6320::Graphics::s_debugMenuSlider->sliderCurrentPosition > 19)
						eae6320::Graphics::s_debugMenuSlider->sliderCurrentPosition = 19;
				}
			}
			else
			{
				if (!(eae6320::Graphics::s_debugMenuSlider->sliderCurrentPosition == 0))
				{
					eae6320::Graphics::s_debugSphere1->m_radius -= radiusOffset;
					eae6320::Graphics::s_debugMenuSlider->sliderCurrentPosition--;
					if (eae6320::Graphics::s_debugMenuSlider->sliderCurrentPosition < 0)
						eae6320::Graphics::s_debugMenuSlider->sliderCurrentPosition = 0;
				}
			}
			eae6320::Graphics::s_debugSphere1->LoadDebugSphere();
			break;
		}

		case 3:
			eae6320::Graphics::s_debugSphere1->m_radius = 20;
			eae6320::Graphics::s_debugMenuSlider->sliderCurrentPosition = 0;
			eae6320::Graphics::s_debugSphere1->LoadDebugSphere();
			break;

		case 4:
			if (eae6320::Graphics::s_toggleFPSCheckBox->m_isChecked == true)
				eae6320::Graphics::s_toggleFPSCheckBox->m_isChecked = false;
			else
				eae6320::Graphics::s_toggleFPSCheckBox->m_isChecked = true;
			break;

		default:
			break;
		}
	}

	// Debug Menu Navigation
	void DebugMenuNavigation()
	{
		if (eae6320::UserInput::IsKeyPressed(VK_UP))
		{
			if (!upPressed)
			{
				if(debugMenuSelection == 0) 
					debugMenuSelection = 4;
				else
					debugMenuSelection -= 1;
			}
			upPressed = true;
		}
		else
			upPressed = false;


		if (eae6320::UserInput::IsKeyPressed(VK_DOWN))
		{
			if (!downPressed)
			{
				if (debugMenuSelection == 4)
					debugMenuSelection = 0;
				else
					debugMenuSelection += 1;
			}
			downPressed = true;
		}
		else
			downPressed = false;

		switch (debugMenuSelection)
		{
		case 0:
			eae6320::Graphics::s_activeMenuItem = eae6320::Graphics::DebugMenuSelection::Text;
			break;
		case 1:
			eae6320::Graphics::s_activeMenuItem = eae6320::Graphics::DebugMenuSelection::CheckBox;
			break;
		case 2:
			eae6320::Graphics::s_activeMenuItem = eae6320::Graphics::DebugMenuSelection::Slider;
			break;
		case 3:
			eae6320::Graphics::s_activeMenuItem = eae6320::Graphics::DebugMenuSelection::Button;
			break;
		case 4:
			eae6320::Graphics::s_activeMenuItem = eae6320::Graphics::DebugMenuSelection::ToggleCam;
			break;
		default:
			break;
		}

		if (eae6320::UserInput::IsKeyPressed(VK_RIGHT))
		{
			if (!rightPressed)
			{
				UpdateSelectedMenuItem(true);
			}
			rightPressed = true;
		}
		else
			rightPressed = false;

		if (eae6320::UserInput::IsKeyPressed(VK_LEFT))
		{
			if (!leftPressed)
			{
				UpdateSelectedMenuItem(false);
			}
			leftPressed = true;
		}
		else
			leftPressed = false;
	}

	bool Initialize(const HWND i_renderingWindow) {
		return eae6320::Graphics::Initialize(i_renderingWindow);
	}

	void Run() {
		eae6320::Time::OnNewFrame();

		EnableDebugMenu();

		if (eae6320::Graphics::s_debugMenuEnabled == false)
		{
			UpdateEntities_vector();
			UpdateSprite();
		}
		else
		{
			DebugMenuNavigation();
		}

		eae6320::Graphics::Render();
	}

	bool ShutDown() {
		return eae6320::Graphics::ShutDown();
	}


}
