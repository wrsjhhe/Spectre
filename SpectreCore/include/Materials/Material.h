#pragma once
#include <string>
#include <vector>
#include "Object.h"
BEGIN_NAMESPACE_SPECTRE

class Material:public Object
{
public:
	SpectreObject(Material)

	friend class Renderer;

	std::string VertexShader;
	std::string FragmentShader;
public:
	~Material();

	const std::vector<uint32_t>& GetVertexSPV() const;

	const std::vector<uint32_t>& GetFragmentSPV() const;

	void CompileSpv();

private:
	Material();

private:
	std::vector<uint32_t> m_VertSpvCache;
	std::vector<uint32_t> m_FragSpvCache;
};

END_NAMESPACE_SPECTRE