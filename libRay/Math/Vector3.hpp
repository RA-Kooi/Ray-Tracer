#ifndef a1271947_5c38_93f7_09ef_379b95f67a48
#define a1271947_5c38_93f7_09ef_379b95f67a48

#include <type_traits>

#include "../API.hpp"

namespace LibRay::Math
{
class LIBRAY_API Vector3
{
public:
	union
	{
		struct
		{
			float x;
			float y;
			float z;
		};

		float v[3];
	};

public:
	Vector3();
	explicit Vector3(float x, float y = 0.f, float z = 0.f);

	Vector3(Vector3 const &other) = default;

	Vector3 &operator=(Vector3 const &other) = default;

	Vector3 operator+() const;
	Vector3 operator-() const;

	Vector3 &operator+=(Vector3 const &other);
	Vector3 &operator-=(Vector3 const &other);
	Vector3 &operator*=(float value);
	Vector3 &operator/=(float value);

	// Componentwise calculations that aren't strictly mathematically defined.
	Vector3 &operator*=(Vector3 const &other);
	Vector3 &operator/=(Vector3 const &other);

	float operator[](int index) const;
	float &operator[](int index);

	Vector3 &Normalize();
	Vector3 Normalized() const;

	float Dot(Vector3 const &other) const;
	Vector3 Cross(Vector3 const &other) const;

	float SquareMagnitude() const;
	float Magnitude() const;

	Vector3 &Abs();
	Vector3 Abs() const;

	float MaxComponent() const;
	float MinComponent() const;

	Vector3 Min(Vector3 const &other) const;
	Vector3 Max(Vector3 const &other) const;

	void Print() const;

	static inline Vector3 Zero();
	static inline Vector3 Up();
	static inline Vector3 Down();
	static inline Vector3 Left();
	static inline Vector3 Right();
	static inline Vector3 Forward();
	static inline Vector3 Backward();
};

LIBRAY_API Vector3 operator+(Vector3 const &lhs, Vector3 const &rhs);
LIBRAY_API Vector3 operator-(Vector3 const &lhs, Vector3 const &rhs);
LIBRAY_API Vector3 operator*(Vector3 const &lhs, float value);
LIBRAY_API Vector3 operator*(float value, Vector3 const &rhs);
LIBRAY_API Vector3 operator/(Vector3 const &lhs, float value);
LIBRAY_API bool operator==(Vector3 const &lhs, Vector3 const &rhs);
LIBRAY_API bool operator!=(Vector3 const &lhs, Vector3 const &rhs);

// Componentwise calculations that aren't strictly mathematically defined.
LIBRAY_API Vector3 operator*(Vector3 const &lhs, Vector3 const &rhs);
LIBRAY_API Vector3 operator/(float value, Vector3 const &rhs);
LIBRAY_API Vector3 operator/(Vector3 const &lhs, Vector3 const &rhs);

static_assert(std::is_copy_constructible_v<Vector3>);
static_assert(std::is_copy_assignable_v<Vector3>);
static_assert(std::is_trivially_copyable_v<Vector3>);

static_assert(std::is_move_constructible_v<Vector3>);
static_assert(std::is_move_assignable_v<Vector3>);
} // namespace LibRay::Math

using vec3 = LibRay::Math::Vector3;

#include "Vector3_impl.hpp"

#endif // a1271947_5c38_93f7_09ef_379b95f67a48
