#pragma once
#include "SpectreDefs.h"
#include <DirectXMath.h>
#include <cmath>
BEGIN_NAMESPACE_SPECTRE

#undef  PI
#define PI 					(3.1415926535897932f)

#define MIN_uint8		((uint8_t)	0x00)
#define	MIN_uint16		((uint16_t)	0x0000)
#define	MIN_uint32		((uint32_t)	0x00000000)
#define MIN_uint64		((uint64_t)	0x0000000000000000)
#define MIN_int8		((int8_t)	-128)
#define MIN_int16		((int16_t)	-32768)
#define MIN_int32		((int32_t)	0x80000000)
#define MIN_int64		((int64_t)	0x8000000000000000)

#define MAX_uint8		((uint8_t)	0xff)
#define MAX_uint16		((uint16_t)	0xffff)
#define MAX_uint32		((uint32_t)	0xffffffff)
#define MAX_uint64		((uint64_t)	0xffffffffffffffff)
#define MAX_int8		((int8_t)	0x7f)
#define MAX_int16		((int16_t)	0x7fff)
#define MAX_int32		((int32_t)	0x7fffffff)
#define MAX_int64		((int64_t)	0x7fffffffffffffff)

template<class T>
static __inline auto RadiansToDegrees(T const& radVal) -> decltype(radVal* (180.f / PI))
{
	return radVal * (180.f / PI);
}

template<class T>
static __inline auto DegreesToRadians(T const& degVal) -> decltype(degVal* (PI / 180.f))
{
	return degVal * (PI / 180.f);
}

//typedef DirectX::XMMATRIX Matrix4x4;
//typedef DirectX::XMVECTOR Vector4;
//
//static __inline Vector4 InitVector(float x,float y,float z,float w)
//{
//	return DirectX::XMVectorSet(x, y, z, w);
//}
//
//static __inline void Matrix_Identity(Matrix4x4& mat)
//{
//	mat = DirectX::XMMatrixIdentity();
//}
//
//static __inline Matrix4x4 Matrix_Perspective(float FovAngleY, float AspectRatio, float NearZ, float FarZ)
//{
//	return  DirectX::XMMatrixPerspectiveFovRH(FovAngleY, AspectRatio, NearZ, FarZ);
//}
//
//static __inline void Matrix_Inverse(Matrix4x4& mat)
//{
//	mat = DirectX::XMMatrixInverse(nullptr,mat);
//}
//
//
//

struct Vector3
{

	float x;
	float y;
	float z;

	Vector3(float inX, float inY, float inZ):
		x(inX),y(inY),z(inZ)
	{

	}
	static const Vector3 UpVector;
};


struct Matrix4x4
{
public:

	enum Style
	{
		EulerAngles,
		AxisAngle,
		Quaternion
	};

	float m[4][4];

	static const Matrix4x4 Identity;

public:
	Matrix4x4() {}

	__inline void Perspective(float fovy, float width, float height, float zNear, float zFar)
	{
		float aspect = width / height;
		float tanHalfFovy = tanf(fovy / 2);

		m[0][0] = 1 / (aspect * tanHalfFovy);	m[0][1] = 0.0f;					m[0][2] = 0.0f;									m[0][3] = 0.0f;
		m[1][0] = 0.0f;							m[1][1] = 1 / (tanHalfFovy);	m[1][2] = 0.0f;									m[1][3] = 0.0f;
		m[2][0] = 0.0f;							m[2][1] = 0.0f;					m[2][2] = zFar / (zFar - zNear);				m[2][3] = 1;
		m[3][0] = 0.0f;							m[3][1] = 0.0f;					m[3][2] = zFar * zNear / (zNear - zFar);		m[3][3] = 0.0f;
	}


	__inline void SetIdentity()
	{
		m[0][0] = 1; m[0][1] = 0;  m[0][2] = 0;  m[0][3] = 0;
		m[1][0] = 0; m[1][1] = 1;  m[1][2] = 0;  m[1][3] = 0;
		m[2][0] = 0; m[2][1] = 0;  m[2][2] = 1;  m[2][3] = 0;
		m[3][0] = 0; m[3][1] = 0;  m[3][2] = 0;  m[3][3] = 1;
	}

	__inline void SetOrigin(float x, float y, float z)
	{
		m[3][0] = x;
		m[3][1] = y;
		m[3][2] = z;
	}

	__inline void AppendRotation(const float degrees, const Vector3& axis, const Vector3* pivot = nullptr)
	{
		Matrix4x4 matrix;
		if (pivot) {
			GetAxisRotation(axis.x, axis.y, axis.z, pivot->x, pivot->y, pivot->z, degrees, matrix);
		}
		else {
			GetAxisRotation(axis.x, axis.y, axis.z, 0, 0, 0, degrees, matrix);
		}
		Append(matrix);
	}

	__inline void GetAxisRotation(float u, float v, float w, const float a, const float b, const float c, const float degress, Matrix4x4& m)
	{
		float rad = degress / 180.0f * PI;

		float u2 = u * u;
		float v2 = v * v;
		float w2 = w * w;
		float l2 = u2 + v2 + w2;
		float l = std::sqrtf(l2);

		u /= l;
		v /= l;
		w /= l;
		u2 /= l2;
		v2 /= l2;
		w2 /= l2;

		float cos = std::cos(rad);
		float sin = std::sin(rad);

		m.m[0][0] = u2 + (v2 + w2) * cos;
		m.m[0][1] = u * v * (1 - cos) + w * sin;
		m.m[0][2] = u * w * (1 - cos) - v * sin;
		m.m[0][3] = 0;

		m.m[1][0] = u * v * (1 - cos) - w * sin;
		m.m[1][1] = v2 + (u2 + w2) * cos;
		m.m[1][2] = v * w * (1 - cos) + u * sin;
		m.m[1][3] = 0;

		m.m[2][0] = u * w * (1 - cos) + v * sin;
		m.m[2][1] = v * w * (1 - cos) - u * sin;
		m.m[2][2] = w2 + (u2 + v2) * cos;
		m.m[2][3] = 0;

		m.m[3][0] = (a * (v2 + w2) - u * (b * v + c * w)) * (1 - cos) + (b * w - c * v) * sin;
		m.m[3][1] = (b * (u2 + w2) - v * (a * u + c * w)) * (1 - cos) + (c * u - a * w) * sin;
		m.m[3][2] = (c * (u2 + v2) - w * (a * u + b * v)) * (1 - cos) + (a * v - b * u) * sin;
		m.m[3][3] = 1;
	}
	__inline void Append(const Matrix4x4& other)
	{
		typedef float Float4x4[4][4];
		const Float4x4& a = *((const Float4x4*)this);
		const Float4x4& b = *((const Float4x4*)&other);

		Float4x4 temp;
		temp[0][0] = a[0][0] * b[0][0] + a[0][1] * b[1][0] + a[0][2] * b[2][0] + a[0][3] * b[3][0];
		temp[0][1] = a[0][0] * b[0][1] + a[0][1] * b[1][1] + a[0][2] * b[2][1] + a[0][3] * b[3][1];
		temp[0][2] = a[0][0] * b[0][2] + a[0][1] * b[1][2] + a[0][2] * b[2][2] + a[0][3] * b[3][2];
		temp[0][3] = a[0][0] * b[0][3] + a[0][1] * b[1][3] + a[0][2] * b[2][3] + a[0][3] * b[3][3];

		temp[1][0] = a[1][0] * b[0][0] + a[1][1] * b[1][0] + a[1][2] * b[2][0] + a[1][3] * b[3][0];
		temp[1][1] = a[1][0] * b[0][1] + a[1][1] * b[1][1] + a[1][2] * b[2][1] + a[1][3] * b[3][1];
		temp[1][2] = a[1][0] * b[0][2] + a[1][1] * b[1][2] + a[1][2] * b[2][2] + a[1][3] * b[3][2];
		temp[1][3] = a[1][0] * b[0][3] + a[1][1] * b[1][3] + a[1][2] * b[2][3] + a[1][3] * b[3][3];

		temp[2][0] = a[2][0] * b[0][0] + a[2][1] * b[1][0] + a[2][2] * b[2][0] + a[2][3] * b[3][0];
		temp[2][1] = a[2][0] * b[0][1] + a[2][1] * b[1][1] + a[2][2] * b[2][1] + a[2][3] * b[3][1];
		temp[2][2] = a[2][0] * b[0][2] + a[2][1] * b[1][2] + a[2][2] * b[2][2] + a[2][3] * b[3][2];
		temp[2][3] = a[2][0] * b[0][3] + a[2][1] * b[1][3] + a[2][2] * b[2][3] + a[2][3] * b[3][3];

		temp[3][0] = a[3][0] * b[0][0] + a[3][1] * b[1][0] + a[3][2] * b[2][0] + a[3][3] * b[3][0];
		temp[3][1] = a[3][0] * b[0][1] + a[3][1] * b[1][1] + a[3][2] * b[2][1] + a[3][3] * b[3][1];
		temp[3][2] = a[3][0] * b[0][2] + a[3][1] * b[1][2] + a[3][2] * b[2][2] + a[3][3] * b[3][2];
		temp[3][3] = a[3][0] * b[0][3] + a[3][1] * b[1][3] + a[3][2] * b[2][3] + a[3][3] * b[3][3];

		memcpy(this, &temp, 16 * sizeof(float));
	}
};
END_NAMESPACE_SPECTRE