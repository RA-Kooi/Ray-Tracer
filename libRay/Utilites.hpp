#ifndef ef3e73f2_6802_a167_a402_e93a7c0ef6b2
#define ef3e73f2_6802_a167_a402_e93a7c0ef6b2

#include <cfloat>
#include <chrono>
#include <string>

#include "API.hpp"

template<typename T> using Observer = T*;

inline bool AlmostEqual(float a, float b, float epsilon = FLT_EPSILON)
{
	float const difference = (a - b);
	return difference <= epsilon && -difference <= epsilon;
}

class LIBRAY_API Stopwatch
{
public:
	void Start();
	void Stop();

	std::string Value() const;

private:
	using Clock = std::chrono::high_resolution_clock;

	Clock::time_point start, end;
};

static_assert(std::is_copy_constructible_v<Stopwatch>);
static_assert(std::is_copy_assignable_v<Stopwatch>);
static_assert(std::is_trivially_copyable_v<Stopwatch>);

static_assert(std::is_move_constructible_v<Stopwatch>);
static_assert(std::is_move_assignable_v<Stopwatch>);

#endif // ef3e73f2_6802_a167_a402_e93a7c0ef6b2
