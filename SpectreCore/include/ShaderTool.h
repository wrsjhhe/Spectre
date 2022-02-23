#pragma once
#include "SpectreDefs.h"
#include "RenderDefs.h"
#include <string>
#include <vector>
BEGIN_NAMESPACE_SPECTRE

class ShaderTool
{
public:
	static std::vector<uint32_t> Compile_glsl(std::string strGlsl,ShaderType type);

};

END_NAMESPACE_SPECTRE