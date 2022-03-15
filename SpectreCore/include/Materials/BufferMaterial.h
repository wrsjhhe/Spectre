#pragma once
#include <string>
#include <memory>
#include <vector>
#include "SpectreApi.h"
#include "RenderTypes.h"

BEGIN_NAMESPACE_SPECTRE
class BufferMaterial;
typedef std::shared_ptr<BufferMaterial> BufferMaterialPtr;
class BufferMaterial :public SpectreApi
{
public:
	DefineClass(BufferMaterial)

	static std::shared_ptr<BufferMaterial> Create();
public:

	void SetVertexShader(std::string vert);
	void SetFragmentShader(std::string frag);

	const std::string& GetVertexShader()const { return m_VertexShader; }
	const std::string& GetFragmentShader()const { return m_FragmentShader; }

	void DefineAttributes(const std::vector<VertexAttribute>& attrs);

	const std::vector<VertexAttribute> GetAttributes() const { return m_Attributes; }

	size_t GetHash();

	~BufferMaterial();


private:
	BufferMaterial();

private:
	std::vector<VertexAttribute> m_Attributes;
	std::string m_VertexShader;
	std::string m_FragmentShader;
};


END_NAMESPACE_SPECTRE