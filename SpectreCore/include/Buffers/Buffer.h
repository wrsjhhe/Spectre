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
	/// ���·����������������newSizeС�ڵ�ǰsize����ʧ��
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
