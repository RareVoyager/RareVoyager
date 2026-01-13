#include <include/util.h>

#include "include/logger/logger.h"

#include <pthread.h>

#include <assert.h>
#if defined(_WIN32)
#include <windows.h>
#include <dbghelp.h>
#elif defined(__APPLE__)
#include <pthread.h>
#include <sys/types.h>
#elif defined(__linux__)
#include <sys/syscall.h>
#include <unistd.h>
#include <sys/types.h>
#include <backtrace.h>
#endif
namespace RareVoyager
{
	// 根据不同的平台选用不同的方法。
	pid_t getThreadPid()
	{
#if defined(_WIN32)
		return GetCurrentThreadId();
#elif defined(__APPLE__)
		uint64_t tid = 0;
		pthread_threadid_np(nullptr, &tid);
		return static_cast<pid_t>(tid);
#elif defined(__linux__)
		return static_cast<pid_t>(::syscall(SYS_gettid));
#endif

	}

	uint32_t getFiberId()
	{
		return 0;
	}

	std::string GetCurrentDateStr()
	{
		time_t t = time(0);
		struct tm tm_time;
#if defined(_WIN32)
		localtime_s(&tm_time, &t);
#else
		localtime_r(&t, &tm_time);
#endif
		char buf[64] = {0};
		strftime(buf, sizeof(buf), "%Y-%m-%d", &tm_time);
		return std::string(buf);
	}

	void Backtrace(std::vector<std::string>& bt, int size, int skip)
	{
#if defined(_WIN32)

#elif defined(__LINUX__)
		void **array = (void **)malloc(size * sizeof(void *));
		size_t s = ::backtrace(array,size);
		char** strings = backtrace_symbols(array,s);
		if (strings == NULL)
		{
			RAREVOYAGER_LOG_INFO(RAREVOYAGER_LOG_ROOT()) << "Backtrace allocation failed";
			return;
		}
		for (size_t i = skip; i < s; ++i)
		{
			bt.push_back(strings[i]);
		}
		free(array);
		free(strings);
#endif
	}
}