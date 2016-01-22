// Header Files
//=============

#include "Game.h"
#include "../../Engine/Time/Time.h"
#include "../../Engine/Math/cVector.h"
#include "../../Engine/Math/cQuaternion.h"
#include "../../Engine/Math/cMatrix_transformation.h"
#include "../../Engine/UserInput/UserInput.h"
#include "../../Engine/Graphics/Graphics.h"
#include <math.h>

namespace Game
{
	bool spaceKeyPressed = false;

	// Helper to update renderable position
	bool UpdateEntities_vector()
	{
		bool wereThereErrors = false;

		eae6320::Math::cVector offset(0.0f, 0.0f, 0.0f);
		eae6320::Math::cVector rotationOffset(0.0f, 0.0f, 0.0f);
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
			}
			// Get the speed
			const float unitsPerSecond = 300.0f;	// This is arbitrary
			const float unitsToMove = unitsPerSecond * eae6320::Time::GetSecondsElapsedThisFrame();	// This makes the speed frame-rate-independent
																									// Normalize the offset
			offset *= unitsToMove;
			rotationOffset *= unitsToMove;
		}
		// The following line assumes there is some "entity" for the rectangle that the game code controls
		// that encapsulates a mesh, an effect, and a position offset.
		// You don't have to do it this way for your assignment!
		// You just need a way to update the position offset associated with the colorful rectangle.
		eae6320::Graphics::s_camera->UpdatePosition(offset);

		// Setting camera rotation once for x axis and once for y axis
		eae6320::Graphics::s_camera->m_orientaion = eae6320::Graphics::s_camera->m_orientaion * eae6320::Math::cQuaternion(rotationOffset.y, eae6320::Math::cVector(0.0f, 1.0f, 0.0f));

		return !wereThereErrors;
	}

	// Helper to update camera position
	bool UpdateCamera_vector()
	{
		bool wereThereErrors = false;

		eae6320::Math::cVector rotationOffset(0.0f, 0.0f, 0.0f);
		{
			// Get the direction
			{
				if (eae6320::UserInput::IsKeyPressed('A'))
				{
					rotationOffset.y -= 0.3f;
				}
				if (eae6320::UserInput::IsKeyPressed('D'))
				{
					rotationOffset.y += 0.3f;
				}
				if (eae6320::UserInput::IsKeyPressed('W'))
				{
					rotationOffset.x -= 0.3f;
				}
				if (eae6320::UserInput::IsKeyPressed('S'))
				{
					rotationOffset.x += 0.3f;
				}
			}
			// Get the speed
			const float unitsPerSecond = 3.0f;	// This is arbitrary
			const float unitsToMove = unitsPerSecond * eae6320::Time::GetSecondsElapsedThisFrame();	// This makes the speed frame-rate-independent
																												// Normalize the offset
			rotationOffset *= unitsToMove;
		}

		// Setting camera rotation once for x axis and once for y axis
		eae6320::Graphics::s_camera->m_orientaion = eae6320::Graphics::s_camera->m_orientaion * eae6320::Math::cQuaternion(rotationOffset.y, eae6320::Math::cVector(0.0f, 1.0f, 0.0f));
		eae6320::Graphics::s_camera->m_orientaion = eae6320::Graphics::s_camera->m_orientaion * eae6320::Math::cQuaternion(rotationOffset.x, eae6320::Math::cVector(1.0f, 0.0f, 0.0f));

		return !wereThereErrors;
	}

	// Helper to detect shooting based on user input
	bool ShootInput()
	{
		bool wereThereErrors = false;

		if (eae6320::UserInput::IsKeyPressed(VK_SPACE))
		{

			spaceKeyPressed = true;
		}
		else
		{
			spaceKeyPressed = false;
		}

		return !wereThereErrors;
	}

	bool Initialize(const HWND i_renderingWindow) {
		return eae6320::Graphics::Initialize(i_renderingWindow);
	}

	void Run() {
		eae6320::Time::OnNewFrame();

		UpdateEntities_vector();
		ShootInput();
		//MoveObjects();

		eae6320::Graphics::Render();
	}

	bool ShutDown() {
		return eae6320::Graphics::ShutDown();
	}


}
