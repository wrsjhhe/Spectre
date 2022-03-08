#pragma once
#include "SpectreApi.h"
#include "Allocator.h"
BEGIN_NAMESPACE_SPECTRE

template<typename T>
class Buffer :public SpectreApi
{
public:
	DefineClass(Buffer);

	void Alloc(uint32_t size)
	{
		m_DataPtr = (T*)Spectre::aligned_alloc(size, sizeof(T));
	}

	/// <summary>
	/// 重新分配内容容量，如果newSize小于当前size，则失败
	/// </summary>
	/// <param name="newSize"></param>
	void Realloc(uint32_t newSize)
	{
		m_DataPtr = (T*)Spectre::aligned_realloc(newSize, sizeof(T));
	}

protected:
	T*							   m_DataPtr;
	uint32_t                       m_Size;
	uint32_t                       m_Capacity;
};

END_NAMESPACE_SPECTRE
