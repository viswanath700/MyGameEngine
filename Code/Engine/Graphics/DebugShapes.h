/*
This file contains the function declarations for graphics
*/

#ifndef DEBUG_SHAPES_H
#define DEBUG_SHAPES_H

// Header Files
//=============
#include "../Math/cVector.h"

// Interface
//==========

namespace eae6320
{
	namespace Graphics
	{
		class DebugLine {

			Math::cVector m_startPoint;
			Math::cVector m_endPoint;
			Math::cVector m_color;

		public:
			DebugLine() {};
			DebugLine(Math::cVector i_start, Math::cVector i_end, Math::cVector i_color = Math::cVector(1.0f, 1.0f, 1.0f)) :
				m_startPoint(i_start),
				m_endPoint(i_end),
				m_color(i_color)
			{
			}
			void DrawLine();
		};

		class DebugBox {
			Math::cVector m_position;
			float m_size;
			Math::cVector m_color;

		public:
			DebugBox() {};
			DebugBox(Math::cVector i_position, float i_size, Math::cVector i_color = Math::cVector(1.0f, 1.0f, 1.0f)):
				m_position(i_position),
				m_size(i_size),
				m_color(i_color)
			{
			}
			void DrawBox();
		};

		class DebugSphere {
			Math::cVector m_center;
			float m_radius;
			size_t m_slices;
			size_t m_stacks;
			Math::cVector m_color;

		public:
			DebugSphere() {};
			DebugSphere(Math::cVector i_center, float i_radius, size_t i_slices, size_t i_stacks, Math::cVector i_color = Math::cVector(1.0f, 1.0f, 1.0f)) :
				m_center(i_center),
				m_radius(i_radius),
				m_slices(i_slices),
				m_stacks(i_stacks),
				m_color(i_color)
			{
			}
			void DrawSphere();
		};
	}
}

#endif	// DEBUG_SHAPES_H