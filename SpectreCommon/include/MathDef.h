#pragma once
#include "CommonDefinitions.h"
#include <DirectXMath.h>

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
//static __inline void Matrix_MoveTo(Matrix4x4& mat,const Vector4& vec)
//{
//	Vector4 a = InitVector(1,1,1,1);
//	Vector4 b = InitVector(1, 1, 1, 1);;
//	Vector4 c = InitVector(1, 1, 1, 1);
//	Vector4 d = vec;
//	DirectX::XMMatrixAffineTransformation(a,b,c,d);
//}
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

};
END_NAMESPACE_SPECTRE