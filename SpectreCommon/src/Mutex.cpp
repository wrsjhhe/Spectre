#include "Mutex.h"

#if SPE_PLATFORM_WINDOWS
#	include <windows.h>
#elif SPE_PLATFORM_LINUX \
	  BX_PLATFORM_ANDROID
#	include <pthread.h>
#endif

USING_NAMESPACE(Spectre)

#if SPE_PLATFORM_WINDOWS
Mutex::Mutex() noexcept
{
	CRITICAL_SECTION* mutex = (CRITICAL_SECTION*)m_internal;
	InitializeCriticalSection(mutex);
}

Mutex::~Mutex() noexcept
{
	CRITICAL_SECTION* mutex = (CRITICAL_SECTION*)m_internal;
	DeleteCriticalSection(mutex);
}

void Mutex::Lock() noexcept
{

	CRITICAL_SECTION* mutex = (CRITICAL_SECTION*)m_internal;
	EnterCriticalSection(mutex);
}

void Mutex::Unlock() noexcept
{
	CRITICAL_SECTION* mutex = (CRITICAL_SECTION*)m_internal;
	LeaveCriticalSection(mutex);
}

#elif SPE_PLATFORM_LINUX \
	  SPE_PLATFORM_ANDROID
enum Enum
	{
		Unlocked,
		Locked,
		Contested,
	};
Mutex::Mutex()
{
	BX_STATIC_ASSERT(sizeof(int32_t) <= sizeof(m_internal));

	uint32_t* futex = (uint32_t*)m_internal;
	*futex = State::Unlocked;
	}

Mutex::~Mutex()
{
}

void Mutex::Lock()
{
	uint32_t* futex = (uint32_t*)m_internal;

	if (State::Unlocked == bx::atomicCompareAndSwap<uint32_t>(futex, State::Unlocked, State::Locked))
	{
		return;
	}

	while (State::Unlocked != bx::atomicCompareAndSwap<uint32_t>(futex, State::Locked, State::Contested))
	{
		crt0::futexWait(futex, State::Contested);
	}
}

void Mutex::Unlock()
{
	uint32_t* futex = (uint32_t*)m_internal;

	if (State::Contested == bx::atomicCompareAndSwap<uint32_t>(futex, State::Locked, State::Unlocked))
	{
		crt0::futexWake(futex, State::Locked);
	}
}
#endif

MutexScope::MutexScope() noexcept
{
	m_mutex.Lock();
}

MutexScope::~MutexScope() noexcept
{
	m_mutex.Unlock();
}