#include "Utilites.hpp"

void Stopwatch::Start()
{
	end = Clock::time_point();
	start = Clock::now();
}

void Stopwatch::Stop()
{
	end = Clock::now();
}

std::string Stopwatch::Value() const
{
	using std::chrono::milliseconds;
	using std::chrono::microseconds;
	using std::chrono::duration_cast;

	Clock::duration duration = end - start;
	float milli = float(duration_cast<milliseconds>(duration).count());
	milli += float(duration_cast<microseconds>(duration).count()) / 1000.f;

	return std::to_string(milli) + "ms";
}
