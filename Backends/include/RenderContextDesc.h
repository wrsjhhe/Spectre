#pragma once
#include <vector>
#include "SpectreDef.h"
#include "RenderDef.h"
#include "NativeWindow.h"

BEGIN_NAMESPACE_SPECTRE

struct RenderContextDesc
{
	bool Headless = false;
	NativeWindow Window;
	std::vector<VertexAttribute> VertexAttrs;
	std::vector<std::vector<uint32_t>> VertexShaders;
	std::vector<std::vector<uint32_t>> FragmentShaders;
};

END_NAMESPACE_SPECTRE