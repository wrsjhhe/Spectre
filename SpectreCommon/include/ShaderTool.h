#pragma once
#include "SpectreDef.h"
#include "RenderTypes.h"
#include <string>
#include <vector>
BEGIN_NAMESPACE_SPECTRE

class ShaderTool
{
public:
	static std::vector<uint32_t> Compile_glsl(std::string strGlsl,ShaderType type);

};

END_NAMESPACE_SPECTRE