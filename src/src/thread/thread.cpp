#include <include/thread/thread.h>
#include <include/util.h>
#include <include/logger/logger.h>


namespace RareVoyager
{
#pragma region Semaphore

	Semaphore::Semaphore(uint32_t count)
	{
		if (sem_init(&m_sem, 0, count))
		{
			throw std::logic_error("sem_init failed");
		}
	}

	Semaphore::~Semaphore()
	{
		sem_destroy(&m_sem);
	}

	void Semaphore::wait()
	{
		if (sem_wait(&m_sem))
		{
			throw std::logic_error("sem_wait failed");
		}

	}

	void Semaphore::notify()
	{
		if (sem_post(&m_sem))
		{
			throw std::logic_error("sem_post failed");
		}
	}

#pragma endregion Semaphore


#pragma region Thread
	static thread_local Thread* t_thread = nullptr;
	static thread_local std::string t_thread_name = "UNKNOW";

	static Logger::ptr system_logger = RAREVOYAGER_LOG_NAME("system");

	Thread* Thread::GetThis()
	{
		return t_thread;
	}

	const std::string& Thread::GetName()
	{
		return t_thread_name;
	}


	void Thread::SetName(const std::string& name)
	{
		if (t_thread)
		{
			t_thread->m_name = name;
		}
		t_thread_name = name;
	}


	Thread::Thread(std::function<void()> cb, const std::string& name) : m_cb(cb)
	{
		if (name.empty())
		{
			m_name = "UNKNOW";
		}
		m_name = name;
		int rt = pthread_create(&m_thread, nullptr, &Thread::run, this);
		if (rt)
		{
			RAREVOYAGER_LOG_ERROR(system_logger) << "pthread_create thread faild rt = " << rt << "thread name = " << name;
			throw std::logic_error("pthread_create thread error");
		}
		m_semaphore.wait();
	}

	Thread::~Thread()
	{
		if (t_thread)
		{
			pthread_detach(m_thread);
		}

	}

	void Thread::join()
	{
		if (m_thread)
		{
			int rt = pthread_join(m_thread, nullptr);
			if (rt)
			{
				RAREVOYAGER_LOG_ERROR(system_logger) << "pthread_join thread faild rt = " << rt << "thread name = " << m_name;
				throw std::logic_error("pthread_join thread error");
			}
			m_thread = 0;
		}
	}

	void* Thread::run(void* arg)
	{
		auto thread = (Thread*)arg;
		t_thread = thread;
		thread->m_id = RareVoyager::getThreadPid();
		t_thread_name = thread->m_name;
		// 最多 16 个字符
		pthread_setname_np(pthread_self(), t_thread_name.substr(0, 15).c_str());
		std::function<void()> cb;

		cb.swap(thread->m_cb);
		thread->m_semaphore.notify();
		if (cb)
		{
			cb();
		}
		return 0;
	}
#pragma endregion Thread

}