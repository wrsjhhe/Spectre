#include "SpectreDef.h"
#include "Materials/BufferMaterial.h"

USING_NAMESPACE(Spectre)


void BufferMaterial::SetVertexShader(std::string vert)
{
	m_VertexShader = vert;
}

void BufferMaterial::SetFragmentShader(std::string frag)
{
	m_FragmentShader = frag;
}

BufferMaterial::BufferMaterial()
{
	
}


size_t BufferMaterial::GetHash()
{
	std::hash<std::string> HashFnStr;
	auto vertHash = HashFnStr(m_VertexShader);
	auto fragHash = HashFnStr(m_FragmentShader);

	return vertHash ^ fragHash;
}

BufferMaterial::~BufferMaterial()
{
	delete m_MaterialBuffer;
}

void BufferMaterial::DefineAttributes(const std::vector<VertexAttribute>& attrs)
{
	m_Attributes = attrs;
}
