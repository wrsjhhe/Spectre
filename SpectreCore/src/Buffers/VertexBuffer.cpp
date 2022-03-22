#include "Buffers/VertexBuffer.h"
#include "Objects/Mesh.h"
#include <memory>
#include <map>
USING_NAMESPACE(Spectre)



VertexBuffer::VertexBuffer()
{

}


VertexBuffer::~VertexBuffer()
{

}

void VertexBuffer::PushMesh(Mesh* pMesh, float* pVertices, uint32_t count)
{
	uint32_t newSize = count * sizeof(float) + m_Size;
	if (newSize > m_Capacity)
	{
		this->Realloc(newSize);
	}

	std::memcpy(m_DataPtr + m_VertexCount, pVertices, sizeof(float) * count);
	m_VertexCount += count;

	pMesh->SetBindVertexBuffer(shared_from_this());
	pMesh->SetVertexBufferOffset(m_Size);
	m_Meshes.push_back(pMesh);
	m_Size = newSize;
}

void VertexBuffer::RemoveMesh(Mesh* pMesh, uint32_t offset, uint32_t count)
{
	assert(offset < m_Size);

	uint32_t offsetBack = offset + count;
	uint32_t moveSize = (m_VertexCount - offsetBack) * sizeof(float);
	std::memmove(m_DataPtr + offset, m_DataPtr + offsetBack, moveSize);
	m_VertexCount -= count;

	m_Size = m_Size - count * sizeof(float);
	uint32_t removeSize = count * sizeof(float);
	if (m_VertexCount == 0)
	{
		Free();
	}
	else
	{
		this->Realloc(m_Size);
	}

	uint32_t index = 0;
	for (; index < m_Meshes.size(); ++index)
	{
		if (m_Meshes[index] == pMesh)
		{
			break;
		}
	}
	m_Meshes.erase(m_Meshes.begin() + index);
	for (; index < m_Meshes.size(); ++index)
	{
		uint32_t offset = m_Meshes[index]->GetVertexBufferOffset();
		offset -= removeSize;
		m_Meshes[index]->SetVertexBufferOffset(offset);
	}
}
