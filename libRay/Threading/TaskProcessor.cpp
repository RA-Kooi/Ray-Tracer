#include "TaskProcessor.hpp"

#include <thread>

namespace LibRay
{
namespace Threading
{
TaskProcessor::TaskProcessor(std::size_t threadCount)
: threadCount(threadCount)
, lastTask(0)
, nextTask(0)
, ready(false)
, tasks()
{
}

void TaskProcessor::AddTask(std::function<void()> func)
{
	tasks.emplace_back(
		[
			this,
			function = std::move(func)
		]()
		{
			while(!ready.load(std::memory_order_acquire))
				std::this_thread::yield();

			function();

			size_t next = nextTask.fetch_add(1, std::memory_order_acq_rel) + 1;
			if(next < tasks.size())
			{
				std::thread t(tasks[next]);

				t.detach();
			}

			lastTask.fetch_add(1, std::memory_order_acq_rel);
		});
}

void TaskProcessor::Run()
{
	for(std::size_t i = 0; i < threadCount && i < tasks.size(); ++i)
	{
		std::thread t(tasks[i]);
		t.detach();
	}

	nextTask.store(
		std::min(threadCount, tasks.size()) - 1, std::memory_order_relaxed);

	ready.store(true, std::memory_order_release);

	while(lastTask.load(std::memory_order_acquire) < tasks.size())
		std::this_thread::yield();
}
} // namespace Threading
} // namespace LibRay
