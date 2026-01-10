/*************************************************
 * 描述：
 *
 * File：mutex.h
 * Author：Cipher
 * Date：2026/1/8-18:57
 * Update：
 * ************************************************/

#ifndef RAREVOYAGER_MUTEX_H
#define RAREVOYAGER_MUTEX_H
#include <atomic>
#include <pthread.h>

namespace RareVoyager
{
	template<typename _ClassT>
	struct ScopedLockImpl
	{
	public:
		ScopedLockImpl(_ClassT* mutex) : m_mutex(mutex)
		{
			m_mutex->lock();
			m_locked = true;
		}

		~ScopedLockImpl()
		{
			unlock();
		}

		void lock()
		{
			if (!m_locked)
			{
				m_mutex->lock();
				m_locked = true;
			}
		}

		void unlock()
		{
			if (m_locked)
			{
				m_mutex->unlock();
				m_locked = false;
			}
		}

	private:
		_ClassT* m_mutex;
		bool m_locked;
	};

	template<typename _ClassT>
	struct ReadScopedLockImpl
	{
	public:
		ReadScopedLockImpl(_ClassT* mutex) : m_rdmutex(mutex)
		{
			m_rdmutex->rdlock();
			m_locked = true;
		}

		~ReadScopedLockImpl()
		{
			unlock();
		}

		void lock()
		{
			if (!m_locked)
			{
				m_rdmutex->unlock();
				m_locked = true;
			}
		}

		void unlock()
		{
			if (m_locked)
			{
				m_rdmutex->unlock();
				m_locked = false;
			}
		}

	private:
		_ClassT* m_rdmutex;
		bool m_locked;
	};

	template<typename _ClassT>
	struct WriteScopedLockImpl
	{
	public:
		WriteScopedLockImpl(_ClassT* mutex) : m_wrmutex(mutex)
		{
			m_wrmutex->wrlock();
			m_locked = true;
		}

		~WriteScopedLockImpl()
		{
			unlock();
		}

		void lock()
		{
			if (!m_locked)
			{
				m_wrmutex->lock();
				m_locked = true;
			}
		}

		void unlock()
		{
			if (m_locked)
			{
				m_wrmutex->unlock();
				m_locked = false;
			}
		}

	private:
		_ClassT* m_wrmutex;
		bool m_locked;
	};

	class Mutex
	{
	public:
		typedef ScopedLockImpl<Mutex> Lock;

		Mutex();

		~Mutex();

		void lock();

		void unlock();

	private:
		pthread_mutex_t m_mutex;
	};

	class RWMutex
	{
	public:
		// 读锁，可能多个线程共享
		typedef ReadScopedLockImpl<RWMutex> ReadLock;
		// 写锁，独占式
		typedef WriteScopedLockImpl<RWMutex> WriteLock;

		RWMutex();

		~RWMutex();

		void rdlock();

		void wrlock();

		void unlock();

	private:
		pthread_rwlock_t m_rwlock;
	};

#pragma region Spinlock
	class Spinlock
	{
	public:
		typedef ScopedLockImpl<Spinlock> Lock;

		Spinlock();

		~Spinlock();

		void lock();

		void unlock();

	private:
		pthread_spinlock_t m_spinlock;
	};
#pragma endregion Spinlock

#pragma region CASLock
	class CASLock
	{
	public:
		typedef ScopedLockImpl<CASLock> Lock;
		CASLock();
		~CASLock();
		void lock();

		void unlock();
	private:
		volatile std::atomic_flag m_locked;
	};
#pragma endregion CASLock
}

#endif //RAREVOYAGER_MUTEX_H