#pragma once
#include <vector>
#include "SpectreDefs.h"
#include "RenderDefs.h"
#include "NativeWindow.h"

BEGIN_NAMESPACE_SPECTRE

struct RenderContextDesc
{
	bool Headless = false;
	NativeWindow Window;
	std::vector<VertexAttribute> VertexAttrs;
};

END_NAMESPACE_SPECTRE