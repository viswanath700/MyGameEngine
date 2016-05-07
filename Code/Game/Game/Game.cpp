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

#include <stdio.h>
#include <string.h>
#include "RakNet/Source/RakPeerInterface.h"
#include "RakNet/Source/MessageIdentifiers.h"
#include "RakNet/Source/BitStream.h"
#include "RakNet/Source/RakNetTypes.h"

#define MAX_CLIENTS 10
#define SERVER_PORT 60000

enum GameMessages
{
	ID_GAME_MESSAGE_1 = ID_USER_PACKET_ENUM + 1
};

struct NetworkPacket
{
	float x, y, z;
	float m_x, m_y, m_z, m_w;
	float fx, fy, fz;
	float bx, by, bz;
	bool bulletOn;
	uint8_t score;
	char type;
};

namespace Game
{
	NetworkPacket data;
	//To handle network messages
	HDC hDeviceContextHandle;
	RECT ClientRectangle;
	PAINTSTRUCT OurPaintStructure;

	RakNet::RakPeerInterface *peer = RakNet::RakPeerInterface::GetInstance();
	bool isServer;
	bool connectionEstablished;
	RakNet::Packet *packet;

	uint8_t pressedNum = 0;
	bool tildePressed = false;
	uint8_t debugMenuSelection = 0;
	bool upPressed = false;
	bool downPressed = false;
	bool rightPressed = false;
	bool leftPressed = false;

	// Capture the Flag
	bool shootBullet;
	float startBulletTime;
	eae6320::Math::cVector bulletOffsetPos;
	eae6320::Math::cVector capturePosition;
	bool capturedFlag;
	bool playerWon;
	bool playerTagged;
	float bulletCollisionOffset;
	float flagCollisionOffset;
	float capturePointOffset;

	void initNetwork()
	{
		printf("(C) or (S)erver?\n");
		//fgets(str, 512, stdin);
		char str[512];
		if (!isServer)
		{
			RakNet::SocketDescriptor sd;
			peer->Startup(1, &sd, 1);
			isServer = false;
		}
		else {
			RakNet::SocketDescriptor sd(SERVER_PORT, 0);
			peer->Startup(MAX_CLIENTS, &sd, 1);
			isServer = true;
		}

		if (isServer)
		{
			printf("Starting the server.\n");
			// We need to let the server accept incoming connections from the clients
			peer->SetMaximumIncomingConnections(MAX_CLIENTS);
		}
		else {
			printf("Enter server IP or hit enter for 127.0.0.1\n");
			/*fgets(str,512,stdin);*/
			strcpy(str, "127.0.0.1");

			/*if (str[0] == 0) {
			strcpy(str, "127.0.0.1");
			}*/
			printf("Starting the client.\n");
			peer->Connect(str, SERVER_PORT, 0, 0);

		}
	}

	void updateNetwork()
	{
		for (packet = peer->Receive(); packet; peer->DeallocatePacket(packet), packet = peer->Receive())
		{
			std::string networkMessage;

			switch (packet->data[0])
			{
			case ID_REMOTE_DISCONNECTION_NOTIFICATION:
				networkMessage = "Another Client has disconnected :";
				break;
			case ID_REMOTE_CONNECTION_LOST:
				networkMessage = "Another Client has lost the connection :";
				printf("Another client has lost the connection.\n");
				break;
			case ID_REMOTE_NEW_INCOMING_CONNECTION:
				printf("Another client has connected.\n");
				break;
			case ID_CONNECTION_REQUEST_ACCEPTED:
			{
				//Client writes data to be sent to server
				networkMessage = "Our connection request has been accepted:";

				//DrawText(hDeviceContextHandle, networkMessage.c_str(), -1, &ClientRectangle, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
				// Use a BitStream to write a custom user message
				// Bitstreams are easier to use than sending casted structures, and handle endian swapping automatically
				RakNet::BitStream bsOut;
				bsOut.Write((RakNet::MessageID)ID_GAME_MESSAGE_1);

				bsOut.Write(eae6320::Graphics::s_snowmanClient->m_position.x);
				bsOut.Write(eae6320::Graphics::s_snowmanClient->m_position.y);
				bsOut.Write(eae6320::Graphics::s_snowmanClient->m_position.z);

				bsOut.Write(eae6320::Graphics::s_snowmanClient->m_orientation.m_w);
				bsOut.Write(eae6320::Graphics::s_snowmanClient->m_orientation.m_x);
				bsOut.Write(eae6320::Graphics::s_snowmanClient->m_orientation.m_y);
				bsOut.Write(eae6320::Graphics::s_snowmanClient->m_orientation.m_z);

				bsOut.Write(eae6320::Graphics::s_flag1->m_position.x);
				bsOut.Write(eae6320::Graphics::s_flag1->m_position.y);
				bsOut.Write(eae6320::Graphics::s_flag1->m_position.z);

				bsOut.Write(eae6320::Graphics::s_bullet2->m_position.x);
				bsOut.Write(eae6320::Graphics::s_bullet2->m_position.y);
				bsOut.Write(eae6320::Graphics::s_bullet2->m_position.z);

				bsOut.Write(eae6320::Graphics::s_bullet2->m_isActive);
				bsOut.Write(eae6320::Graphics::s_snowmanClientScore->m_score);

				networkMessage = "Sending message from client to server";

				//DrawText(hDeviceContextHandle, networkMessage.c_str(), -1, &ClientRectangle, DT_SINGLELINE | DT_CENTER | DT_VCENTER);		
				peer->Send(&bsOut, HIGH_PRIORITY, UNRELIABLE, 0, packet->systemAddress, false);
			}
			break;
			case ID_NEW_INCOMING_CONNECTION:
				networkMessage = "A connection is incoming";

				//DrawText(hDeviceContextHandle, networkMessage.c_str(), -1, &ClientRectangle, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
				printf("A connection is incoming.\n");
				break;
			case ID_NO_FREE_INCOMING_CONNECTIONS:
				printf("The server is full.\n");
				break;
			case ID_DISCONNECTION_NOTIFICATION:
				if (isServer) {
					printf("A client has disconnected.\n");
				}
				else {
					printf("We have been disconnected.\n");
				}
				break;
			case ID_CONNECTION_LOST:
				if (isServer) {
					printf("A client lost the connection.\n");
				}
				else {
					printf("Connection lost.\n");
				}
				break;

			case ID_GAME_MESSAGE_1:
			{
				//Server reading from client or Client reading from server
				RakNet::RakString rs;
				float x, y, z;
				float m_w, m_x, m_y, m_z;
				float fx, fy, fz;
				float bx, by, bz;
				bool bulletOn;
				uint8_t score;
				connectionEstablished = true;

				RakNet::BitStream bsIn(packet->data, packet->length, false);
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
				//This will read the camera position of the client, so that we can set the client object's position from here
				bsIn.Read(x);
				bsIn.Read(y);
				bsIn.Read(z);

				bsIn.Read(m_w);
				bsIn.Read(m_x);
				bsIn.Read(m_y);
				bsIn.Read(m_z);

				bsIn.Read(fx);
				bsIn.Read(fy);
				bsIn.Read(fz);

				bsIn.Read(bx);
				bsIn.Read(by);
				bsIn.Read(bz);

				bsIn.Read(bulletOn);
				bsIn.Read(score);

				bsIn.Read(rs);
				printf("%s\n", rs.C_String());

				data.x = x; data.y = y; data.z = z;
				data.m_w = m_w; data.m_x = m_x; data.m_y = m_y; data.m_z = m_z;
				data.fx = fx; data.fy = fy; data.fz = fz;
				data.bx = bx; data.by = by; data.bz = bz;
				data.bulletOn = bulletOn;
				data.score = score;

				RakNet::BitStream bsOut;
				//This will send the current Server/Client 's camera position
				bsOut.Write((RakNet::MessageID)ID_GAME_MESSAGE_1);
				if (isServer) {
					bsOut.Write(eae6320::Graphics::s_snowman->m_position.x);
					bsOut.Write(eae6320::Graphics::s_snowman->m_position.y);
					bsOut.Write(eae6320::Graphics::s_snowman->m_position.z);

					bsOut.Write(eae6320::Graphics::s_snowman->m_orientation.m_w);
					bsOut.Write(eae6320::Graphics::s_snowman->m_orientation.m_x);
					bsOut.Write(eae6320::Graphics::s_snowman->m_orientation.m_y);
					bsOut.Write(eae6320::Graphics::s_snowman->m_orientation.m_z);

					bsOut.Write(eae6320::Graphics::s_flag2->m_position.x);
					bsOut.Write(eae6320::Graphics::s_flag2->m_position.y);
					bsOut.Write(eae6320::Graphics::s_flag2->m_position.z);

					bsOut.Write(eae6320::Graphics::s_bullet1->m_position.x);
					bsOut.Write(eae6320::Graphics::s_bullet1->m_position.y);
					bsOut.Write(eae6320::Graphics::s_bullet1->m_position.z);

					bsOut.Write(eae6320::Graphics::s_bullet1->m_isActive);
					bsOut.Write(eae6320::Graphics::s_snowmanScore->m_score);


					bsOut.Write("Server to Client");
					networkMessage = "Sending from server to client";

				}
				else {
					bsOut.Write(eae6320::Graphics::s_snowmanClient->m_position.x);
					bsOut.Write(eae6320::Graphics::s_snowmanClient->m_position.y);
					bsOut.Write(eae6320::Graphics::s_snowmanClient->m_position.z);

					bsOut.Write(eae6320::Graphics::s_snowmanClient->m_orientation.m_w);
					bsOut.Write(eae6320::Graphics::s_snowmanClient->m_orientation.m_x);
					bsOut.Write(eae6320::Graphics::s_snowmanClient->m_orientation.m_y);
					bsOut.Write(eae6320::Graphics::s_snowmanClient->m_orientation.m_z);

					bsOut.Write(eae6320::Graphics::s_flag1->m_position.x);
					bsOut.Write(eae6320::Graphics::s_flag1->m_position.y);
					bsOut.Write(eae6320::Graphics::s_flag1->m_position.z);

					bsOut.Write(eae6320::Graphics::s_bullet2->m_position.x);
					bsOut.Write(eae6320::Graphics::s_bullet2->m_position.y);
					bsOut.Write(eae6320::Graphics::s_bullet2->m_position.z);

					bsOut.Write(eae6320::Graphics::s_bullet2->m_isActive);
					bsOut.Write(eae6320::Graphics::s_snowmanClientScore->m_score);

					bsOut.Write("Client to Server");
					networkMessage = "Sending from Client to server";

				}

				//DrawText(hDeviceContextHandle, networkMessage.c_str(), -1, &ClientRectangle, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
				peer->Send(&bsOut, HIGH_PRIORITY, UNRELIABLE, 0, packet->systemAddress, false);
			}
			break;

			default:
				printf("Message with identifier %i has arrived.\n", packet->data[0]);
				break;
			}
		}
	}

	void destroyNetwork()
	{
		RakNet::RakPeerInterface::DestroyInstance(peer);

	}

	void ThirdPersonMovement(eae6320::Graphics::GameObject* player, eae6320::Math::cVector posOffset, eae6320::Math::cVector rotOffset, eae6320::Math::cVector thirdPersonOffset)
	{
		// Rotating Player
		player->m_orientation = player->m_orientation *
			eae6320::Math::cQuaternion(rotOffset.y, eae6320::Math::cVector(0.0f, 1.0f, 0.0f));

		// Updating player's position
		eae6320::Math::cVector oldSnowmanPos = player->m_position;
		eae6320::Math::cMatrix_transformation i_localToWorldTransformSnowman = eae6320::Math::cMatrix_transformation(
			eae6320::Graphics::s_camera->m_orientation, player->m_position);
		eae6320::Math::cVector newSnowmanPos = eae6320::Math::cMatrix_transformation::matrixMulVector(i_localToWorldTransformSnowman, posOffset);
		player->m_position = newSnowmanPos;

		// Updating third person camera according to the player's position
		eae6320::Math::cVector camOffset = eae6320::Graphics::s_camera->m_offsetFromPlayer;
		eae6320::Math::cVector val = eae6320::Math::cMatrix_transformation::matrixMulVector(i_localToWorldTransformSnowman, camOffset);
		eae6320::Graphics::s_camera->m_position += (val - eae6320::Graphics::s_camera->m_position) * eae6320::Time::GetSecondsElapsedThisFrame() * 3;
		eae6320::Graphics::s_camera->m_orientation = player->m_orientation *
			eae6320::Math::cQuaternion(rotOffset.y, eae6320::Math::cVector(0.0f, 1.0f, 0.0f));

		// Temporarily moving third person camera left/right
		eae6320::Math::cMatrix_transformation i_localToWorldTransformCamera = eae6320::Math::cMatrix_transformation(
			eae6320::Graphics::s_camera->m_orientation, eae6320::Graphics::s_camera->m_position);
		eae6320::Graphics::s_camera->m_position = eae6320::Math::cMatrix_transformation::matrixMulVector(i_localToWorldTransformCamera, thirdPersonOffset);

		// Updating debug line for the player
		oldSnowmanPos.y += 35;
		newSnowmanPos.y = oldSnowmanPos.y;
		if (!(newSnowmanPos == oldSnowmanPos))
		{
			eae6320::Math::cVector directionVector = (newSnowmanPos - oldSnowmanPos);
			directionVector.Normalize();
			eae6320::Graphics::s_snowmanLine->m_startPoint = newSnowmanPos;
			eae6320::Graphics::s_snowmanLine->m_endPoint = newSnowmanPos + (directionVector * 100);
			eae6320::Graphics::s_snowmanLine->LoadDebugLine();
		}
		else
		{
			eae6320::Graphics::s_snowmanLine->m_startPoint = eae6320::Graphics::s_snowmanLine->m_endPoint = oldSnowmanPos;
			eae6320::Graphics::s_snowmanLine->LoadDebugLine();
		}
	}

	void CaptureTheFlag(eae6320::Graphics::GameObject* player, eae6320::Graphics::GameObject* flag, eae6320::Graphics::GameObject* bullet, uint8_t& score)
	{
		bool gameReset = false;
		if (isServer)
		{
			capturePosition = eae6320::Math::cVector(1015, 0, -1088);
		}
		else
		{
			capturePosition = eae6320::Math::cVector(-1015, 0, 1088);
		}

		//check for flag
		if (player->m_position.x < flag->m_position.x + flagCollisionOffset && player->m_position.x > flag->m_position.x - flagCollisionOffset
			&& player->m_position.z < flag->m_position.z + flagCollisionOffset && player->m_position.z > flag->m_position.z - flagCollisionOffset)
		{
			capturedFlag = true;
		}

		//attach flag
		if (capturedFlag && !playerWon && !playerTagged)
		{
			flag->m_position = player->m_position;
		}

		//check for score zone
		if (player->m_position.x < capturePosition.x + capturePointOffset && player->m_position.x > capturePosition.x - capturePointOffset
			&& player->m_position.z < capturePosition.z + capturePointOffset && player->m_position.z > capturePosition.z - capturePointOffset && capturedFlag && !playerWon)
		{
			playerWon = true;
			flag->m_position = flag->m_defaultPosition;
			score++;
			gameReset = true;
			//gameReset = Graphics::dogaObject->Reset();
		}

		//check for tag
		if (player->m_position.x < bullet->m_position.x + bulletCollisionOffset && player->m_position.x > bullet->m_position.x - bulletCollisionOffset
			&& player->m_position.z < bullet->m_position.z + bulletCollisionOffset && player->m_position.z > bullet->m_position.z - bulletCollisionOffset
			&& capturedFlag && !playerWon && bullet->m_isActive)
		{
			playerTagged = true;
			flag->m_position = flag->m_defaultPosition;
			gameReset = true;
			//gameReset = Graphics::dogaObject->ResetFlag();

		}

		if (gameReset)
		{
			playerWon = false;
			capturedFlag = false;
			playerTagged = false;
		}
	}

	void ShootBullet(eae6320::Graphics::GameObject* bullet, eae6320::Graphics::GameObject* player, int shootKey)
	{
		eae6320::Math::cVector bulletOffset(0.0f, 0.0f, 0.0f);
		if (eae6320::UserInput::IsKeyPressed(shootKey) && !shootBullet)
		{
			bullet->m_isActive = true;
			bullet->m_position = player->m_position + bulletOffsetPos;
			startBulletTime = eae6320::Time::GetTotalSecondsElapsed();
			shootBullet = true;
		}

		if (shootBullet)
		{
			bulletOffset.z -= 3.0f;
			bulletOffset.y -= 0.1f;
			bulletOffset *= 300 * eae6320::Time::GetSecondsElapsedThisFrame();
			eae6320::Math::cMatrix_transformation mat = eae6320::Math::cMatrix_transformation::cMatrix_transformation(player->m_orientation, bullet->m_position);
			bullet->m_position = (eae6320::Math::cMatrix_transformation::matrixMulVector(mat, bulletOffset));
		}

		if ((eae6320::Time::GetTotalSecondsElapsed() - startBulletTime) > 1 && shootBullet)
		{
			//stop bullet
			shootBullet = false;
			bullet->m_position = player->m_position + bulletOffsetPos;
			bullet->m_isActive = false;
		}
	}

	// Helper to update renderable position
	bool UpdateEntities_vector()
	{
		bool wereThereErrors = false;

		eae6320::Math::cVector offset(0.0f, 0.0f, 0.0f);
		eae6320::Math::cVector rotationOffset(0.0f, 0.0f, 0.0f);
		eae6320::Math::cVector thirdPersonCamOffset(0.0f, 0.0f, 0.0f);
		
		float unitsPerSecond = 300.0f;	// This is arbitrary
		
		{
			if(isServer)
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
					thirdPersonCamOffset.x -= 5.0f;
				}
				if (eae6320::UserInput::IsKeyPressed('X'))
				{
					thirdPersonCamOffset.x += 5.0f;
				}
				
				// Sprint
				if (eae6320::UserInput::IsKeyPressed(VK_LSHIFT) && eae6320::Graphics::s_sprintBar->barPosition > 0)
				{
					unitsPerSecond = 500.0f;
					eae6320::Graphics::s_sprintBar->barPosition -= 0.1f;
				}
				else if (eae6320::Graphics::s_sprintBar->barPosition < 150)
				{
					unitsPerSecond = 200.0f;
					eae6320::Graphics::s_sprintBar->barPosition += 0.02f;
				}
			}
			else
			{
				if (eae6320::UserInput::IsKeyPressed('J'))
				{
					offset.x -= 1.0f;
				}
				if (eae6320::UserInput::IsKeyPressed('L'))
				{
					offset.x += 1.0f;
				}
				if (eae6320::UserInput::IsKeyPressed('I'))
				{
					offset.z -= 1.0f;
				}
				if (eae6320::UserInput::IsKeyPressed('K'))
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

				if (eae6320::UserInput::IsKeyPressed('N'))
				{
					thirdPersonCamOffset.x -= 5.0f;
				}
				if (eae6320::UserInput::IsKeyPressed('M'))
				{
					thirdPersonCamOffset.x += 5.0f;
				}

				// Sprint
				if (eae6320::UserInput::IsKeyPressed(VK_RSHIFT) && eae6320::Graphics::s_sprintBar->barPosition > 0)
				{
					unitsPerSecond = 500.0f;
					eae6320::Graphics::s_sprintBar->barPosition -= 0.1f;
				}
				else if (eae6320::Graphics::s_sprintBar->barPosition < 150)
				{
					unitsPerSecond = 200.0f;
					eae6320::Graphics::s_sprintBar->barPosition += 0.02f;
				}
			}

			// Get the speed
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
		else // Third person camera movement
		{
			rotationOffset.y = offset.x / 300.0f;
			offset.x = 0;
			offset.y = 0;

			//ThirdPersonMovement(eae6320::Graphics::s_snowman, offset, rotationOffset, thirdPersonCamOffset);

			if (isServer && connectionEstablished)
			{
				//I am server
				eae6320::Graphics::s_snowmanClient->m_position.x = data.x;
				eae6320::Graphics::s_snowmanClient->m_position.y = data.y;
				eae6320::Graphics::s_snowmanClient->m_position.z = data.z;

				eae6320::Graphics::s_snowmanClient->m_orientation.m_w = data.m_w;
				eae6320::Graphics::s_snowmanClient->m_orientation.m_x = data.m_x;
				eae6320::Graphics::s_snowmanClient->m_orientation.m_y = data.m_y;
				eae6320::Graphics::s_snowmanClient->m_orientation.m_z = data.m_z;
			}
			else if (!isServer && connectionEstablished) {
				//I am client
				eae6320::Graphics::s_snowman->m_position.x = data.x;
				eae6320::Graphics::s_snowman->m_position.y = data.y;
				eae6320::Graphics::s_snowman->m_position.z = data.z;

				eae6320::Graphics::s_snowman->m_orientation.m_w = data.m_w;
				eae6320::Graphics::s_snowman->m_orientation.m_x = data.m_x;
				eae6320::Graphics::s_snowman->m_orientation.m_y = data.m_y;
				eae6320::Graphics::s_snowman->m_orientation.m_z = data.m_z;
			}

			if(isServer)
				ThirdPersonMovement(eae6320::Graphics::s_snowman, offset, rotationOffset, thirdPersonCamOffset);
			else
				ThirdPersonMovement(eae6320::Graphics::s_snowmanClient, offset, rotationOffset, thirdPersonCamOffset);
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

	bool Initialize(const HWND i_renderingWindow, bool serverState)
	{
		// Networking
		isServer = serverState;
		initNetwork();

		// Capture the Flag
		startBulletTime = 0;
		capturedFlag = false;
		flagCollisionOffset = 100;
		capturePointOffset = 500;
		bulletCollisionOffset = 40;
		playerWon = false;
		shootBullet = false;
		bulletOffsetPos = eae6320::Math::cVector(0, 40, 0);

		return eae6320::Graphics::Initialize(i_renderingWindow);
	}

	void Run() {
		eae6320::Time::OnNewFrame();

		if (eae6320::UserInput::IsKeyPressed(VK_SPACE))
			eae6320::Graphics::s_clientJoined = true;

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

		if (isServer)
		{
			eae6320::Graphics::DoCOllisions(eae6320::Graphics::s_snowman);

			ShootBullet(eae6320::Graphics::s_bullet1, eae6320::Graphics::s_snowman, VK_LCONTROL);
			CaptureTheFlag(eae6320::Graphics::s_snowman, eae6320::Graphics::s_flag2, eae6320::Graphics::s_bullet2, eae6320::Graphics::s_snowmanScore->m_score);

			eae6320::Graphics::s_flag1->m_position.x = data.fx;
			eae6320::Graphics::s_flag1->m_position.y = data.fy;
			eae6320::Graphics::s_flag1->m_position.z = data.fz;

			eae6320::Graphics::s_bullet2->m_position.x = data.bx;
			eae6320::Graphics::s_bullet2->m_position.y = data.by;
			eae6320::Graphics::s_bullet2->m_position.z = data.bz;

			eae6320::Graphics::s_bullet2->m_isActive = data.bulletOn;
			eae6320::Graphics::s_snowmanClientScore->m_score = data.score;
		}
		else
		{
			eae6320::Graphics::DoCOllisions(eae6320::Graphics::s_snowmanClient);

			ShootBullet(eae6320::Graphics::s_bullet2, eae6320::Graphics::s_snowmanClient, VK_RCONTROL);
			CaptureTheFlag(eae6320::Graphics::s_snowmanClient, eae6320::Graphics::s_flag1, eae6320::Graphics::s_bullet1, eae6320::Graphics::s_snowmanClientScore->m_score);

			eae6320::Graphics::s_flag2->m_position.x = data.fx;
			eae6320::Graphics::s_flag2->m_position.y = data.fy;
			eae6320::Graphics::s_flag2->m_position.z = data.fz;

			eae6320::Graphics::s_bullet1->m_position.x = data.bx;
			eae6320::Graphics::s_bullet1->m_position.y = data.by;
			eae6320::Graphics::s_bullet1->m_position.z = data.bz;

			eae6320::Graphics::s_bullet1->m_isActive = data.bulletOn;
			eae6320::Graphics::s_snowmanScore->m_score = data.score;
		}

		updateNetwork();

		if (isServer)
		{
			
		}
		else
		{

		}

		eae6320::Graphics::Render();
	}

	bool ShutDown() {
		return eae6320::Graphics::ShutDown();
	}


}
