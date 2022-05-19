#pragma once
#include "SpectreDef.h"
#include "RenderTypes.h"

BEGIN_NAMESPACE_SPECTRE

struct Vertex
{
	static std::vector<VertexAttribute> VertexAttributes;
	float Position[3] = { 0.f,0.f,0.f };
	float Color[3] = { 0.f,0.f,0.f };
	float TextureCoord[2] = { 0.f,0.f };
};


END_NAMESPACE_SPECTRE