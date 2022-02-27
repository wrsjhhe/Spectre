#include "Cameras/PerspectiveCamera.h"

USING_NAMESPACE(Spectre)
#undef near;
#undef far;
PerspectiveCamera::PerspectiveCamera(float fov, float aspect, float near, float far):
	m_Fov(fov),
	m_Aspect(aspect),
	m_Near(near),
	m_Far(far)
{
	m_Projection = Matrix::CreatePerspectiveFieldOfView(m_Fov, aspect, m_Near, m_Far);
}


void PerspectiveCamera::LookAt(const Vector3& pos,const Vector3& target, const Vector3& up)
{
	m_World = Matrix::CreateLookAt(pos, target, up);
	m_NeedUpdate = true;
}

Matrix PerspectiveCamera::GetViewProjection()
{
	if (m_NeedUpdate)
	{
		m_ViewProjection = m_World * m_Projection;
		m_NeedUpdate = false;
	}

	return m_ViewProjection;
}
