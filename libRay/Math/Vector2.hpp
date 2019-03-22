#ifndef b0b14a2c_bd1f_e101_4dec_fe439e14a3ad
#define b0b14a2c_bd1f_e101_4dec_fe439e14a3ad

#include <type_traits>

#include "../API.hpp"

namespace LibRay::Math
{
class LIBRAY_API Vector2
{
public:
#pragma warning(push)
#pragma warning(disable : 4201)
	union {
		struct
		{
			float x;
			float y;
		};

		float v[2];
	};
#pragma warning(pop)

public:
	Vector2();
	explicit Vector2(float x, float y = 0.f);

	Vector2(Vector2 const &other) = default;

	Vector2 &operator=(Vector2 const &other) = default;

	Vector2 operator+() const;
	Vector2 operator-() const;

	Vector2 &operator+=(Vector2 const &other);
	Vector2 &operator-=(Vector2 const &other);
	Vector2 &operator*=(float value);
	Vector2 &operator/=(float value);

	// Componentwise calculations that aren't strictly mathematically defined.
	Vector2 &operator*=(Vector2 const &other);
	Vector2 &operator/=(Vector2 const &other);

	float operator[](int index) const;
	float &operator[](int index);

	Vector2 &Normalize();
	Vector2 Normalized() const;

	float Dot(Vector2 const &other) const;
	float SquareMagnitude() const;
	float Magnitude() const;

	Vector2 &Abs();
	Vector2 Abs() const;

	float MaxComponent() const;
	float MinComponent() const;

	Vector2 Min(Vector2 const &other) const;
	Vector2 Max(Vector2 const &other) const;

	void Print() const;

	static inline Vector2 Zero();
	static inline Vector2 Up();
	static inline Vector2 Down();
	static inline Vector2 Left();
	static inline Vector2 Right();
};

LIBRAY_API Vector2 operator+(const Vector2 &lhs, const Vector2 &rhs);
LIBRAY_API Vector2 operator-(const Vector2 &lhs, const Vector2 &rhs);
LIBRAY_API Vector2 operator*(const Vector2 &lhs, float value);
LIBRAY_API Vector2 operator*(float value, Vector2 const &rhs);
LIBRAY_API Vector2 operator/(const Vector2 &lhs, float value);
LIBRAY_API bool operator==(const Vector2 &lhs, const Vector2 &rhs);
LIBRAY_API bool operator!=(const Vector2 &lhs, const Vector2 &rhs);

// Componentwise calculations that aren't strictly mathematically defined.
LIBRAY_API Vector2 operator*(Vector2 const &lhs, Vector2 const &rhs);
LIBRAY_API Vector2 operator/(float value, Vector2 const &rhs);
LIBRAY_API Vector2 operator/(Vector2 const &lhs, Vector2 const &rhs);

static_assert(std::is_copy_constructible_v<Vector2>);
static_assert(std::is_copy_assignable_v<Vector2>);
static_assert(std::is_trivially_copyable_v<Vector2>);

static_assert(std::is_move_constructible_v<Vector2>);
static_assert(std::is_move_assignable_v<Vector2>);
} // namespace LibRay::Math

using vec2 = LibRay::Math::Vector2;

#include "Vector2_impl.hpp"

#endif // b0b14a2c_bd1f_e101_4dec_fe439e14a3ad
