#pragma once
#include "Camera.h"
#include "Math/MathDef.h"
BEGIN_NAMESPACE_SPECTRE

class PerspectiveCamera:public Camera
{
public:
	PerspectiveCamera(float fov,float aspect, float near,float far);

	//void SetPosition(const Vector3& pos);

	void LookAt(const Vector3& pos,const Vector3& target, const Vector3& up);

	Matrix GetViewProjection();

private:
	float		m_Aspect = PI / 4.0f;;
	float		m_Fov = 1.f;
	float       m_Near = 1.0f;
	float       m_Far = 3000.0f;

	bool        m_NeedUpdate = false;

	Vector3		m_Position;
	Matrix		m_World;
	Matrix		m_Projection;
	Matrix		m_ViewProjection;
};


END_NAMESPACE_SPECTRE