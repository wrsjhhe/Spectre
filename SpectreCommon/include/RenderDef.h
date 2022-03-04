#pragma once
#include "SpectreDef.h"
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
	VertexAttribute_None = 0x00000000,
	VertexAttribute_Position = 0x00000001,
	VertexAttribute_UV0 = 0x00000002,
	VertexAttribute_UV1 = 0x00000004,
	VertexAttribute_Normal = 0x00000008,
	VertexAttribute_Tangent = 0x00000010,
	VertexAttribute_Color = 0x00000020,
	VertexAttribute_SkinWeight = 0x00000040,
	VertexAttribute_SkinIndex = 0x00000080,
	VertexAttribute_SkinPack = 0x00000100,
	VertexAttribute_InstanceFloat1 = 0x00000200,
	VertexAttribute_InstanceFloat2 = 0x00000400,
	VertexAttribute_InstanceFloat3 = 0x00000800,
	VertexAttribute_InstanceFloat4 = 0x00001000,
	VertexAttribute_Custom0 = 0x00002000,
	VertexAttribute_Custom1 = 0x00004000,
	VertexAttribute_Custom2 = 0x00008000,
	VertexAttribute_Custom3 = 0x00010000,
	VertexAttribute_Count = 0x00020000,
}VertexAttribute;

typedef enum ShaderType
{
	ShaderType_Vertex = 1,
	ShaderType_Fragment = 2,
	ShaderType_Compute = 3,
	ShaderType_Geometry = 4,
	ShaderType_Tess_control = 5,
	ShaderType_Tess_evaluation = 6
}ShaderType;

END_NAMESPACE_SPECTRE