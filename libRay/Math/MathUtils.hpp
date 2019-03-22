#ifndef d87f1eb6_95e6_e6b1_0838_5639d5d05409
#define d87f1eb6_95e6_e6b1_0838_5639d5d05409

#include <algorithm>
#include <cmath>
#include <tuple>

namespace LibRay::Math
{
constexpr float const PI = 3.14159265358979323846f;

inline float Radians(float degrees)
{
	return degrees * (PI / 180.f);
}

inline float Degrees(float radians)
{
	return radians * (180.f / PI);
}

template<typename T> int Sign(T val)
{
	return (T(0) < val) - (T(0) > val);
}

template<typename T> T Clamp(T val, T min, T max)
{
	return std::max(std::min(val, max), min);
}

inline std::tuple<int, float, float> SolveQuadratic(
	float a,
	float b,
	float c)
{
	float const discriminant = b * b - 4 * a * c;

	if(discriminant < 0)
		return std::make_tuple(0, 0.f, 0.f);

	float const twoA = 2 * a;

	if(discriminant < FLT_EPSILON)
	{
		float const result = -b / twoA;
		return std::make_tuple(1, result, 0.f);
	}

	float const discriminantRoot = std::sqrt(discriminant);

	float const result1 = (-b + discriminantRoot) / twoA;
	float const result2 = (-b - discriminantRoot) / twoA;

	return std::make_tuple(2, result1, result2);
}
} // namespace LibRay::Math

#endif // d87f1eb6_95e6_e6b1_0838_5639d5d05409
