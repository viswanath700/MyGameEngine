/*
This file contains the function declarations for UserOutput
*/

#ifndef EAE6320_CAMERAOBJECT_H
#define EAE6320_CAMERAOBJECT_H

// Header Files
//=============
#include "../Math/cVector.h"
#include "../Math/cQuaternion.h"

// Interface
//==========

namespace eae6320
{
	namespace Graphics
	{
		class CameraObject
		{

		public:
			Math::cVector m_position;
			Math::cQuaternion m_orientaion;

		public:
			CameraObject() :
				m_position(Math::cVector(0, 0, 10)),
				m_orientaion(Math::cQuaternion())
			{
			};

			void UpdatePosition(Math::cVector i_position_offset);
		};

	}
}

#endif	// EAE6320_CAMERAOBJECT_H