#pragma once
#include <string>
#include <memory>
#include <vector>
#include "Object.h"
#include "RenderDef.h"

BEGIN_NAMESPACE_SPECTRE

class BufferMaterial :public Object
{
public:
	SpectreObject(BufferMaterial)

	static std::shared_ptr<BufferMaterial> Create();
public:
	std::string VertexShader;
	std::string FragmentShader;

	void DefineAttributes(const std::vector<VertexAttribute>& attrs);


	~BufferMaterial();


private:
	BufferMaterial();

private:
	std::vector<VertexAttribute> m_Attributes;
};
typedef std::shared_ptr<BufferMaterial> BufferMaterialPtr;

END_NAMESPACE_SPECTRE