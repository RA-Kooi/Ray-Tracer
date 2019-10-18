#include "TaskProcessor.hpp"

#include <algorithm>
#include <cassert>
#include <thread>

namespace LibRay
{
namespace Threading
{
TaskProcessor::TaskProcessor(std::size_t threadCount)
: threadCount(threadCount)
, nextTask(0)
#ifdef DEBUG
, ready(false)
#endif
, tasks()
{
}

void TaskProcessor::AddTask(std::function<void()> func)
{
	tasks.push_back(std::move(func));
}

void TaskProcessor::Run()
{
	auto const taskProcessor =
		[this]()
		{
#ifdef DEBUG
			while(!ready.load(std::memory_order_relaxed))
				std::this_thread::yield();
#endif

			while(true)
			{
				size_t next = nextTask.fetch_add(1, std::memory_order_relaxed);
				if(next < tasks.size())
					tasks[next]();
				else
					break;
			}
		};

	std::vector<std::thread> threads;
	threads.reserve(std::min(threadCount, tasks.size()));

	for(std::size_t i = 0; i < threadCount && i < tasks.size(); ++i)
	{
		threads.emplace_back(taskProcessor);
	}

#ifdef DEBUG
	ready.store(true, std::memory_order_relaxed);
#endif

	for(std::thread &thread: threads)
	{
		if(thread.joinable())
			thread.join();
	}

	tasks.clear();
}
} // namespace Threading
} // namespace LibRay
