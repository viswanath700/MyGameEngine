// Header Files
//=============
#include "GameObject.h"
#include "Graphics.h"
#include "../Math/cMatrix_transformation.h"

// Interface
//==========

eae6320::Graphics::GameObject::GameObject(char* const i_path_mesh, char* const i_path_material)
{
	m_mesh = Mesh(i_path_mesh);
	m_material = Material(i_path_material);

	m_position = Math::cVector();
	m_orientation = Math::cQuaternion();
	m_velocity = Math::cVector(0, 0, 0);
	m_collider.sExtends = Math::cVector(0, 0, 0);

	m_alpha = 1.0;
}

bool eae6320::Graphics::GameObject::LoadObject()
{
	if (!m_material.LoadMaterial())
	{
		return false;
	}

	if(!m_mesh.LoadMesh())
	{
		return false;
	}
	return true;
}

void eae6320::Graphics::GameObject::DrawObject()
{
	m_material.SetMaterial();
	m_material.SetMaterialUniforms();
	m_material.SetEngineUniforms(Math::cMatrix_transformation(m_orientation, m_position), *eae6320::Graphics::s_camera);

	m_mesh.DrawMesh();
}

void eae6320::Graphics::GameObject::UpdatePosition(Math::cVector i_position_offset)
{
	m_position += i_position_offset;
}

void eae6320::Graphics::GameObject::ReleaseGameObject()
{
	m_mesh.ReleaseMesh();
	m_material.ReleaseMaterial();
}