#pragma once
#include "VulkanCommon.h"

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

enum BufferType
{
	Buffer_Type_Undefined = -1,
	Buffer_Type_Host = 1,
	Buffer_Type_Device = 2
};

END_NAMESPACE_SPECTRE