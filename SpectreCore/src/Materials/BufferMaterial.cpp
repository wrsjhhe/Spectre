#include "SpectreDef.h"
#include "Materials/BufferMaterial.h"


USING_NAMESPACE(Spectre)


std::shared_ptr<BufferMaterial> BufferMaterial::Create()
{
	auto* pMat = new BufferMaterial;
	return std::shared_ptr<BufferMaterial>(pMat);
}

BufferMaterial::BufferMaterial()
{

}

BufferMaterial::~BufferMaterial()
{

}

void BufferMaterial::DefineAttributes(const std::vector<VertexAttribute>& attrs)
{
	m_Attributes = attrs;
}
