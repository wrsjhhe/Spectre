#pragma once
#include "SpectreDefs.h"
#include <string>
#include <vector>
BEGIN_NAMESPACE_SPECTRE

class ShaderTool
{
public:
	enum ShaderType
	{
		Vertex = 1,
		Fragment = 2,
		Compute = 3,
		Geometry = 4,
		Tess_control = 5,
		Tess_evaluation = 6
	};

public:
	static std::vector<uint32_t> Compile_glsl(std::string strGlsl,ShaderType type);

};

END_NAMESPACE_SPECTRE