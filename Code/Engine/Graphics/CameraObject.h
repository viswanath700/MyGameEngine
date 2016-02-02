/*
This file contains the function declarations for UserOutput
*/

#ifndef EAE6320_CAMERAOBJECT_H
#define EAE6320_CAMERAOBJECT_H

// Header Files
//=============
#include "../Math/cVector.h"
#include "../Math/cQuaternion.h"
#include "../Math/Functions.h"

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
			Math::cQuaternion m_orientation;
			float m_fieldOfView_y;
			float m_aspectRatio;
			float m_z_nearPlane;
			float m_z_farPlane;

		public:
			CameraObject() :
				m_position(Math::cVector(0, 0, 10)),
				m_orientation(Math::cQuaternion()),
				m_fieldOfView_y(Math::ConvertDegreesToRadians(60)),
				m_aspectRatio((float)800 / 600),
				m_z_nearPlane(0.1f),
				m_z_farPlane(10000)
			{
			};

			void UpdatePosition(Math::cVector i_position_offset);
			void UpdateOrientation(Math::cVector i_rotation_offset);
		};

	}
}

#endif	// EAE6320_CAMERAOBJECT_H