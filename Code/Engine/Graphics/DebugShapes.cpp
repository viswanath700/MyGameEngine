// Header Files
//=============
#include "DebugShapes.h"

#include "../../External/OpenGlExtensions/OpenGlExtensions.h"
#include <gl/GL.h>
#include <gl/GLU.h>
//#include "D3D9Types.h"

// Interface
//==========

void eae6320::Graphics::DebugLine::DrawLine()
{
	/*D3DVECTOR i_start = D3DVECTOR();
	D3DVECTOR i_end = D3DVECTOR();
	D3DCOLOR i_color = D3DCOLOR_XRGB((int)(m_color.x * 255), (int)(m_color.y * 255), (int)(m_color.z * 255));

	i_start.x = m_startPoint.x;
	i_start.y = m_startPoint.y;
	i_start.z = m_startPoint.z;

	i_end.x = m_endPoint.x;
	i_end.y = m_endPoint.y;
	i_end.z = m_endPoint.z;*/



	//glDisable(GL_TEXTURE_2D);
	//glDisable(GL_LIGHTING);
	//glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	//glEnable(GL_COLOR_MATERIAL);

	glLineWidth(2.5f);
	GLfloat mycolour[3] = { 0,0,1 };
	glColor3fv(mycolour);
	glShadeModel(GL_SMOOTH);
	glBegin(GL_LINES);
	//{
		glColor3f(0.0f, 1.0f, 0.0f);
		glVertex3f(m_startPoint.x, m_startPoint.y, m_startPoint.z);
		glColor3f(0.0f, 1.0f, 0.0f);
		glVertex3f(m_endPoint.x, m_endPoint.y, m_endPoint.z);
	//}
	glEnd();


	//glEnable(GL_TEXTURE_2D);
	//glEnable(GL_LIGHTING);
	//glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	//glDisable(GL_COLOR_MATERIAL);


}

void eae6320::Graphics::DebugBox::DrawBox()
{
	
}

void eae6320::Graphics::DebugSphere::DrawSphere()
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glColor3f(0, 1, 0);
	glTranslatef(m_center.x, m_center.y, m_center.z);
	gluSphere(gluNewQuadric(), m_radius, m_slices, m_stacks);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}