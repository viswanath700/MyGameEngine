// Header Files
//=============
#include "CameraObject.h"
#include "../Math/cMatrix_transformation.h"

// Interface
//==========

void eae6320::Graphics::CameraObject::UpdatePosition(Math::cVector i_position_offset)
{
	m_position += i_position_offset;
}