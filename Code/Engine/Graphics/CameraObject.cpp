// Header Files
//=============
#include "CameraObject.h"
#include "../Math/cMatrix_transformation.h"
#include <math.h>

// Interface
//==========

void eae6320::Graphics::CameraObject::UpdatePosition(Math::cVector i_position_offset)
{
	float angleAroundY = 2 * asin(m_orientaion.m_y);

	float x = i_position_offset.x * cos(angleAroundY) - i_position_offset.z * sin(angleAroundY);
	float z = i_position_offset.x * sin(angleAroundY) + i_position_offset.z * cos(angleAroundY);

	m_position += Math::cVector(x, i_position_offset.y, z);
}