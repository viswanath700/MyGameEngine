// Header Files
//=============
#include "CameraObject.h"
#include "../Math/cMatrix_transformation.h"

// Interface
//==========

void eae6320::Graphics::CameraObject::UpdatePosition(Math::cVector i_position_offset)
{
	// To move the camera in the direction it is pointing at
	Math::cMatrix_transformation i_localToWorldTransform = Math::cMatrix_transformation(m_orientation, m_position);
	m_position = Math::cMatrix_transformation::matrixMulVector(i_localToWorldTransform, i_position_offset);
}

void eae6320::Graphics::CameraObject::UpdateOrientation(Math::cVector i_rotation_offset)
{
	m_orientation = m_orientation * eae6320::Math::cQuaternion(i_rotation_offset.x, eae6320::Math::cVector(1.0f, 0.0f, 0.0f));
	m_orientation = m_orientation * eae6320::Math::cQuaternion(i_rotation_offset.y, eae6320::Math::cVector(0.0f, 1.0f, 0.0f));
	m_orientation = m_orientation * eae6320::Math::cQuaternion(i_rotation_offset.z, eae6320::Math::cVector(0.0f, 0.0f, 1.0f));
}