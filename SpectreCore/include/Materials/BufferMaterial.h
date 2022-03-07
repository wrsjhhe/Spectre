#pragma once
#include <string>
#include <memory>
#include <vector>
#include "SpectreApi.h"
#include "RenderTypes.h"

BEGIN_NAMESPACE_SPECTRE

class BufferMaterial :public SpectreApi
{
public:
	DefineClass(BufferMaterial)

	static std::shared_ptr<BufferMaterial> Create();
public:
	std::string VertexShader;
	std::string FragmentShader;

	void DefineAttributes(const std::vector<VertexAttribute>& attrs);

	const std::vector<VertexAttribute> GetAttributes() const { return m_Attributes; }

	~BufferMaterial();


private:
	BufferMaterial();

private:
	std::vector<VertexAttribute> m_Attributes;
};
typedef std::shared_ptr<BufferMaterial> BufferMaterialPtr;

END_NAMESPACE_SPECTRE