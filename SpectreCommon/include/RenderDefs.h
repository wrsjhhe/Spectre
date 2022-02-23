#pragma once
#include "SpectreDefs.h"
BEGIN_NAMESPACE_SPECTRE

struct SwapChainDesc
{
	uint32_t                  Width = 0;
	uint32_t                  Height = 0;
};

typedef enum CommandPoolTypes
{
	CommandPool_Type_Graphics = 1,
	CommandPool_Type_Compute = 2,
	CommandPool_Type_Transfer = 3,
}CommandPoolTypes;

typedef enum BufferType
{
	Buffer_Type_Undefined = -1,
	Buffer_Type_Host = 1,
	Buffer_Type_Device = 2
}BufferType;

typedef enum VertexAttribute
{
	VertexAttribute_None = 0,
	VertexAttribute_Position,
	VertexAttribute_UV0,
	VertexAttribute_UV1,
	VertexAttribute_Normal,
	VertexAttribute_Tangent,
	VertexAttribute_Color,
	VertexAttribute_SkinWeight,
	VertexAttribute_SkinIndex,
	VertexAttribute_SkinPack,
	VertexAttribute_InstanceFloat1,
	VertexAttribute_InstanceFloat2,
	VertexAttribute_InstanceFloat3,
	VertexAttribute_InstanceFloat4,
	VertexAttribute_Custom0,
	VertexAttribute_Custom1,
	VertexAttribute_Custom2,
	VertexAttribute_Custom3,
	VertexAttribute_Count,
}VertexAttribute;

typedef enum ShaderType
{
	Vertex = 1,
	Fragment = 2,
	Compute = 3,
	Geometry = 4,
	Tess_control = 5,
	Tess_evaluation = 6
}ShaderType;

END_NAMESPACE_SPECTRE