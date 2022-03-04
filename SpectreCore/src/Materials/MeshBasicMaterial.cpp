#include "SpectreDef.h"
#include "Materials/MeshBasicMaterial.h"
#include "ShaderTool.h"
USING_NAMESPACE(Spectre)


std::shared_ptr<MeshBasicMaterial> MeshBasicMaterial::Create()
{
	auto* pMat = new MeshBasicMaterial;
	return std::shared_ptr<MeshBasicMaterial>(pMat);
}

MeshBasicMaterial::MeshBasicMaterial()
{

}

MeshBasicMaterial::~MeshBasicMaterial()
{

}

const std::vector<uint32_t>& MeshBasicMaterial::GetVertexSPV() const
{
	return m_VertSpvCache;
}

const std::vector<uint32_t>& MeshBasicMaterial::GetFragmentSPV() const
{
	return m_FragSpvCache;
}

void MeshBasicMaterial::CompileSpv()
{
	if (!VertexShader.empty())
		m_VertSpvCache = ShaderTool::Compile_glsl(VertexShader, ShaderType_Vertex);
	if (!FragmentShader.empty())
		m_FragSpvCache = ShaderTool::Compile_glsl(FragmentShader, ShaderType_Fragment);
}


