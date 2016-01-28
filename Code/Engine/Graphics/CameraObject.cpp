// Header Files
//=============
#include "CameraObject.h"
#include "../Math/cMatrix_transformation.h"
#include "../Math/Functions.h"
#include <math.h>

// Interface
//==========

void eae6320::Graphics::CameraObject::UpdatePosition(Math::cVector i_position_offset)
{
	float angleAroundY = 2 * asin(m_orientaion.m_y);

	float twoPi = 2 * eae6320::Math::Pi;

	/*if(angleAroundY > twoPi)
		angleAroundY = */

	float x = i_position_offset.x * cos(angleAroundY) - i_position_offset.z * sin(angleAroundY);
	float z = i_position_offset.x * sin(angleAroundY) + i_position_offset.z * cos(angleAroundY);

	m_position += Math::cVector(x, i_position_offset.y, z);
}

void eae6320::Graphics::CameraObject::UpdateOrientation(Math::cVector i_rotation_offset)
{
	m_orientaion = m_orientaion * eae6320::Math::cQuaternion(i_rotation_offset.y, eae6320::Math::cVector(0.0f, 1.0f, 0.0f));
}