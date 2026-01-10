/*************************************************
 * 描述：
 *
 * File：thread.h
 * Author：Cipher
 * Date：2026/1/8-10:00
 * Update：
 * ************************************************/

#ifndef RAREVOYAGER_THREAD_H
#define RAREVOYAGER_THREAD_H
#include <string>
#include <semaphore.h>

extern "C" {
#include <pthread.h>
}

#include <thread>
#include <functional>
#include <memory>

#ifdef _WIN32
#include <windows.h>
using tid_t = DWORD;
#else
#include <sys/types.h>
using tid_t = pid_t;
#endif

namespace RareVoyager
{
#pragma region Semaphore
	class Semaphore
	{
	public:
		Semaphore(uint32_t count = 0);

		~Semaphore();

		void wait();

		void notify();

	private:
		Semaphore(const Semaphore&);

		Semaphore(const Semaphore&&) = delete;

		Semaphore& operator=(const Semaphore&) = delete;

	private:
		sem_t m_sem;

	};
#pragma endregion Semaphore

#pragma region Thread
	/**
	 * @brief: 使用pthread 实现多线程，以及互斥量
	 */
	class Thread
	{
	public:
		typedef std::shared_ptr<Thread> ptr;

		Thread(std::function<void()> cb, const std::string& name);

		~Thread();

		void join();

		const std::string& getName() const { return m_name; }
		pid_t getId() const { return m_id; }

		static void* run(void* arg);

		static Thread* GetThis();

		static const std::string& GetName();

		static void SetName(const std::string& name);

	private:
		Thread(const Thread&) = delete;

		Thread(const Thread&&) = delete;

		Thread& operator=(const Thread&) = delete;

	private:
		pid_t m_id = -1;
		pthread_t m_thread = 0;
		std::function<void()> m_cb;
		std::string m_name;

		Semaphore m_semaphore;
	};
#pragma endregion Thread
}


#endif //RAREVOYAGER_THREAD_H