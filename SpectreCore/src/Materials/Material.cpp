#include "SpectreDefs.h"
#include "Materials/Material.h"
#include "ShaderTool.h"
USING_NAMESPACE(Spectre)


Material::Material()
{

}

Material::~Material()
{

}

const std::vector<uint32_t>& Material::GetVertexSPV() const
{
	return m_VertSpvCache;
}

const std::vector<uint32_t>& Material::GetFragmentSPV() const
{
	return m_FragSpvCache;
}

void Material::CompileSpv()
{
	if(!VertexShader.empty())
		m_VertSpvCache = ShaderTool::Compile_glsl(VertexShader, ShaderType_Vertex);
	if (!FragmentShader.empty())
		m_FragSpvCache = ShaderTool::Compile_glsl(FragmentShader, ShaderType_Fragment);
}


