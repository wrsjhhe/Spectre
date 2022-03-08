#include "Buffers/IndexBuffer.h"
#include "Objects/Mesh.h"
#include <memory>

USING_NAMESPACE(Spectre)

IndexBuffer::IndexBuffer()
{

}

IndexBuffer::~IndexBuffer()
{

}

void IndexBuffer::PushMesh(Mesh* pMesh, uint32_t* pIndices, uint32_t count)
{
	uint32_t newSize = count * sizeof(uint32_t) + m_Size;
	if (newSize > m_Capacity)
	{
		this->Realloc(newSize);
	}

	std::memcpy(m_DataPtr + m_IndexCount, pIndices, sizeof(uint32_t) * count);
	m_IndexCount += count;

	pMesh->SetBindIndexBuffer(shared_from_this());
	pMesh->SetVertexBufferOffset(m_Size);
	m_Meshes.push_back(pMesh);
	m_Size = newSize;
}

void IndexBuffer::RemoveMesh(Mesh* pMesh, uint32_t offset, uint32_t count)
{
	assert(offset < m_Size);

	uint32_t offsetBack = offset + count;
	uint32_t moveSize = (m_IndexCount - offsetBack) * sizeof(float);
	std::memmove(m_DataPtr + offset, m_DataPtr + offsetBack, moveSize);
	m_IndexCount -= count;

	m_Size = m_Size - count * sizeof(float);
	uint32_t removeSize = count * sizeof(float);
	if (m_IndexCount == 0)
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
		uint32_t offset = m_Meshes[index]->GetIndexBufferOffset();
		offset -= removeSize;
		m_Meshes[index]->SetIndexBufferOffset(offset);
	}
}
