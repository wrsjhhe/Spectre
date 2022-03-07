#pragma once
#include <string>
#include <vector>
#include <memory>
#include "Object.h"
BEGIN_NAMESPACE_SPECTRE

class MeshBasicMaterial :public Object
{
public:
	SpectreObject(MeshBasicMaterial)

	std::string VertexShader;
	std::string FragmentShader;

	static std::shared_ptr<MeshBasicMaterial> Create();
public:
	
	~MeshBasicMaterial();

	const std::vector<uint32_t>& GetVertexSPV() const;

	const std::vector<uint32_t>& GetFragmentSPV() const;

	void CompileSpv();

private:
	MeshBasicMaterial();

private:
	std::vector<uint32_t> m_VertSpvCache;
	std::vector<uint32_t> m_FragSpvCache;
};

typedef std::shared_ptr<MeshBasicMaterial> MeshBasicMaterialPtr;
END_NAMESPACE_SPECTRE