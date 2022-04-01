#pragma once
#include "SpectreDef.h"
#include "SimpleMath.h"


#undef  PI
#define PI 				DirectX::XM_PI

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

BEGIN_NAMESPACE_SPECTRE

typedef DirectX::SimpleMath::Vector2 Vector2;
typedef DirectX::SimpleMath::Vector3 Vector3;
typedef DirectX::SimpleMath::Vector4 Vector4;
typedef DirectX::SimpleMath::Matrix Matrix;
typedef DirectX::SimpleMath::Color Color;

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


struct Vertex
{
	float Position[3];
	float Color[3];
};

END_NAMESPACE_SPECTRE