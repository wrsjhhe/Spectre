#include "Cameras/PerspectiveCamera.h"

USING_NAMESPACE(Spectre)

PerspectiveCamera::PerspectiveCamera(float fov, float aspect, float near, float far):
	m_Fov(fov),
	m_Aspect(aspect),
	m_Near(near),
	m_Far(far)
{
	m_Projection = Matrix::CreatePerspectiveFieldOfView(m_Fov, aspect, m_Near, m_Far);
}

void PerspectiveCamera::SetPosition(const Vector3& pos)
{
	m_Position = pos;
}

void PerspectiveCamera::LookAt(const Vector3& target, const Vector3& up)
{
	Matrix::CreateLookAt(m_Position, target, up);
}
