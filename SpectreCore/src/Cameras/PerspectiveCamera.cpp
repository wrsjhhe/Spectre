#include "Cameras/PerspectiveCamera.h"

USING_NAMESPACE(Spectre)

PerspectiveCamera::PerspectiveCamera(float fov, float aspect, float near, float far):
	m_Fov(fov),
	m_Aspect(aspect),
	m_Near(near),
	m_Far(far)
{
	//m_Projection.Perspective(m_Fov, aspect, m_Near, m_Far);

	//Matrix4x4 view = m_World
	//m_ViewProjection = 
}
