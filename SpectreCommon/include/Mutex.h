#pragma once
#include <atomic>
#include "CommonDefinitions.h"
#include "Platform.h"



BEGIN_NAMESPACE_COMMON

class Mutex
{
public:
	Mutex() noexcept;
	Mutex(const Mutex&) = delete;
	Mutex& operator=(const Mutex&) = delete;

	~Mutex() noexcept;
	void Lock() noexcept;

	void Unlock() noexcept;

private:
	__declspec(align(16)) uint8_t m_internal[64];
};

class MutexScope
{
	MutexScope() = delete;
	MutexScope(const MutexScope&) = delete;
	MutexScope& operator=(const MutexScope&) = delete;

public:
	///
	MutexScope(Mutex& _mutex);

	///
	~MutexScope();

private:
	Mutex& m_mutex;
};
END_NAMESPACE_COMMON