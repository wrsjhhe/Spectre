#include "SpectreDef.h"
#include "Materials/MeshBasicMaterial.h"
#include "ShaderTool.h"
#include "FileUtils.h"
USING_NAMESPACE(Spectre)


std::shared_ptr<MeshBasicMaterial> MeshBasicMaterial::Create()
{
	auto* pMat = new MeshBasicMaterial;
	return std::shared_ptr<MeshBasicMaterial>(pMat);
}

void MeshBasicMaterial::EnableVextexColor(bool enable)
{
	ShaderVariable* sv = reinterpret_cast<ShaderVariable*>(m_MaterialBuffer);
	sv->UserVextexColor = enable?1:0;
}

void MeshBasicMaterial::SetColor(Color rgb)
{
	auto v3 = rgb.ToVector3();
	v3 /= 255.f;
	ShaderVariable* sv = reinterpret_cast<ShaderVariable*>(m_MaterialBuffer);
	std::memcpy(sv->Color, &v3, sizeof(sv->Color));
}

MaterialBufferInfo MeshBasicMaterial::GetBufferInfo()
{
	return {
		sizeof(ShaderVariable),
		m_MaterialBuffer
	};
}


MeshBasicMaterial::MeshBasicMaterial()
{
	SetVertexShader(FileUtils::ReadFile("Shaders/triangle.vert"));
	SetFragmentShader(FileUtils::ReadFile("Shaders/triangle.frag"));

	std::vector<VertexAttribute> VertexAttrs = { VertexAttribute_Position, VertexAttribute_Color };
	DefineAttributes(VertexAttrs);
	m_MaterialBuffer = new ShaderVariable{
		0,
		{255.f,255.f,255.f}
	};

}


MeshBasicMaterial::~MeshBasicMaterial()
{

}


