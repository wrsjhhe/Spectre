#pragma once
#include "SpectreApi.h"
#include "Allocator.h"
BEGIN_NAMESPACE_SPECTRE

template<typename T>
class Buffer :public SpectreApi
{
private:
	constexpr uint32_t lowSize = 1024 * 1024 * 10;
	constexpr uint32_t highSize = 1024 * 1024 * 100;
public:
	DefineClass(Buffer);

	Buffer()
	{

	}
	virtual ~Buffer()
	{
		Free();
	}
	explicit Buffer(uint32_t size)
	{
		Alloc(size);
	}

	void Alloc(uint32_t size)
	{
		if (m_DataPtr != nullptr)
			Free();
		m_DataPtr = (T*)Spectre::aligned_alloc(size, sizeof(T));
	}

	/// <summary>
	/// 重新分配内容容量
	/// </summary>
	/// <param name="newSize"></param>
	void Realloc(uint32_t newSize)
	{
		if (m_DataPtr == nullptr)
		{
			Alloc(newSize);
			return;
		}

		if (newSize > m_Size)
		{
			if (newSize <= m_Capacity)
				return;

			if (newSize <= lowSize)
			{
				newSize = newSize * 3;
			}
			else if (newSize > lowSize && newSize < highSize)
			{
				newSize = newSize * 2;
			}
			else
			{
				newSize = newSize * 1.5;
			}

			m_DataPtr = (T*)Spectre::aligned_realloc(newSize, sizeof(T));
			m_Capacity = newSize;
		}
		else if (newSize < m_Size)
		{
			if (newSize < 0.5 * m_Capacity)
			{
				m_DataPtr = (T*)Spectre::aligned_realloc(0.5 * m_Capacity, sizeof(T));
				m_Capacity = 0.5 * m_Capacity;
			}
		}
	}

	void Free()
	{
		Spectre::aligned_free(m_DataPtr);
		m_DataPtr = nullptr;
		m_Size = 0;
		m_Capacity = 0;
	}

protected:
	T*							   m_DataPtr = nullptr;
	uint32_t                       m_Size = 0;
	uint32_t                       m_Capacity = 0;
};

END_NAMESPACE_SPECTRE
