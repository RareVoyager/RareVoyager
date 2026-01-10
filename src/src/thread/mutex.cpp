#include <include/thread/mutex.h>

namespace RareVoyager
{
	/**
	 * @brief: 读写锁的具体实现s
	 */
#pragma region RWMutex
	RWMutex::RWMutex()
	{
		pthread_rwlock_init(&m_rwlock, nullptr);
	}

	RWMutex::~RWMutex()
	{
		pthread_rwlock_destroy(&m_rwlock);
	}

	void RWMutex::rdlock()
	{
		// 核心是共享
		pthread_rwlock_rdlock(&m_rwlock);
	}

	void RWMutex::wrlock()
	{
		// 核心是独占
		pthread_rwlock_wrlock(&m_rwlock);
	}

	void RWMutex::unlock()
	{
		pthread_rwlock_unlock(&m_rwlock);
	}
#pragma endregion RWMutex

#pragma region Mutex
	Mutex::Mutex()
	{
		pthread_mutex_init(&m_mutex, nullptr);
	}

	Mutex::~Mutex()
	{
		pthread_mutex_destroy(&m_mutex);
	}

	void Mutex::lock()
	{
		pthread_mutex_lock(&m_mutex);
	}

	void Mutex::unlock()
	{
		pthread_mutex_unlock(&m_mutex);
	}
#pragma endregion	Mutex

#pragma region Spinlock
	Spinlock::Spinlock()
	{
		pthread_spin_init(&m_spinlock, PTHREAD_PROCESS_PRIVATE);
	}

	Spinlock::~Spinlock()
	{
		pthread_spin_destroy(&m_spinlock);
	}

	void Spinlock::lock()
	{
		pthread_spin_lock(&m_spinlock);
	}
	void Spinlock::unlock()
	{
		pthread_spin_unlock(&m_spinlock);
	}
#pragma endregion Spinlock

#pragma region CASLock
	CASLock::CASLock()
	{
		m_locked.clear();
	}

	CASLock::~CASLock()
	{
	}

	void CASLock::lock()
	{
		while (std::atomic_flag_test_and_set_explicit(&m_locked, std::memory_order_acquire));
	}

	void CASLock::unlock()
	{
		std::atomic_flag_clear_explicit(&m_locked, std::memory_order_release);
	}
#pragma endregion CASLock

}