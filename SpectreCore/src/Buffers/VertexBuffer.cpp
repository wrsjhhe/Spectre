#include "Buffers/VertexBuffer.h"
USING_NAMESPACE(Spectre)


void VertexBuffer::Push(float* pVertices, uint32_t size)
{
	uint64_t requireSize = size + m_Size;
	if (requireSize > m_Capacity)
	{
		if(requireSize >)
	}
}

void VertexBuffer::Remove(uint32_t offset, uint32_t size)
{

}
