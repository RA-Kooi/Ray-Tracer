#ifndef ce567da0_3c8f_fa60_64ba_d496c09d6b21
#define ce567da0_3c8f_fa60_64ba_d496c09d6b21

#include <atomic>
#include <condition_variable>
#include <cstddef>
#include <functional>
#include <mutex>
#include <type_traits>
#include <vector>

namespace LibRay
{
namespace Threading
{
class TaskProcessor final
{
public:
	TaskProcessor(std::size_t threadCount);

	void AddTask(std::function<void()> func);

	void Run();

private:
	std::size_t threadCount;

	std::atomic_size_t lastTask, nextTask;
	std::atomic_bool ready;

	std::vector<std::function<void()>> tasks;
};

static_assert(!std::is_copy_constructible_v<TaskProcessor>);
static_assert(!std::is_copy_assignable_v<TaskProcessor>);
static_assert(!std::is_trivially_copyable_v<TaskProcessor>);

static_assert(!std::is_move_constructible_v<TaskProcessor>);
static_assert(!std::is_move_assignable_v<TaskProcessor>);
} // namespace Threading
} // namespace LibRay

#endif // ce567da0_3c8f_fa60_64ba_d496c09d6b21
