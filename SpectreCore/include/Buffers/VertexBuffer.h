#pragma once
#include "Buffer.h"

BEGIN_NAMESPACE_SPECTRE

class VertexBuffer :public Buffer<float>
{
public:
	DefineClass(VertexBuffer);

	void Push(float* pVertices, uint32_t size);

	void Remove(uint32_t offset, uint32_t size);

private:
	float* m_VerticesData;
};

END_NAMESPACE_SPECTRE