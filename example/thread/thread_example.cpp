#include "include/thread/mutex.h"

#include <include/thread/thread.h>
#include <include/logger/logger.h>
#include <include/util.h>

volatile static int count = 0;
RareVoyager::Mutex mutex;
void func1()
{
	RAREVOYAGER_LOG_INFO(RAREVOYAGER_LOG_ROOT()) << "name = " << RareVoyager::Thread::GetName() << std::endl
												 << "this.name = " << RareVoyager::Thread::GetThis()->GetName()<< std::endl
												 << "id = " << RareVoyager::getThreadPid()<< std::endl
												 << "this.id = " << RareVoyager::Thread::GetThis()->getId();
	for (int i = 0; i < 10000000; i++)
	{
		RareVoyager::Mutex::Lock lock(&mutex);
		++count;
	}
}

void func2()
{

}

int main()
{
	std::vector<RareVoyager::Thread::ptr> threads;
	for (int i = 0; i < 5; i++)
	{
		auto* thread = new RareVoyager::Thread(&func1, "name_" + std::to_string(i));
		threads.emplace_back(thread);
	}

	for (int i = 0; i < 5; i++)
	{
		threads[i]->join();
	}
	RAREVOYAGER_LOG_INFO(RAREVOYAGER_LOG_ROOT()) << "thread test end";
	RAREVOYAGER_LOG_INFO(RAREVOYAGER_LOG_ROOT()) << count;
	return 0;
}