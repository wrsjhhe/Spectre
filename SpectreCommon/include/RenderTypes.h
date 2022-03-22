#pragma once
#include "SpectreDef.h"
#include <vector>
#include <string>
BEGIN_NAMESPACE_SPECTRE

typedef enum CommandPoolTypes
{
	CommandPool_Type_Graphics = 1,
	CommandPool_Type_Compute = 2,
	CommandPool_Type_Transfer = 3,
}CommandPoolTypes;


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

typedef enum BufferType
{
	BufferType_Transfer_Src = 0x00000001,
	BufferType_Transfer_Dst = 0x00000002,
	BufferType_Uniform_Texel = 0x00000004,
	BufferType_Storage_Texel = 0x00000008,
	BufferType_Uniform = 0x00000010,
	BufferType_Storage = 0x00000020,
	BufferType_Index = 0x000000040,
	BufferType_Vertex = 0x00000080,
	BufferType_Indirect = 0x00000100,
	BufferType_Shader_Device_Address = 0x00020000,
}BufferType;

typedef enum MemoryProperty
{
	MemoryProperty_Device_Local = 0x00000001,
	MemoryProperty_Host_Visible = 0x00000002,
	MemoryProperty_Host_Coherent = 0x00000004,
	MemoryProperty_Host_Cached = 0x00000008,
	MemoryProperty_Lazily_Allocated = 0x00000010,
	MemoryProperty_Protected = 0x00000020,
	MemoryProperty_Device_Coherentamd = 0x00000040,
	MemoryProperty_Device_Uncached_amd = 0x00000080,
	MemoryProperty_Rdma_Capable_Nv = 0x00000100,
	MemoryProperty_Flag_Max = 0x7fFFFFFF
};

struct SwapChainDesc
{
	uint32_t                  Width = 0;
	uint32_t                  Height = 0;
};


END_NAMESPACE_SPECTRE