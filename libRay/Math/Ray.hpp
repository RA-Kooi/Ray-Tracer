#ifndef fa606def_a0a5_4c2c_8152_2fd70f16f4b1
#define fa606def_a0a5_4c2c_8152_2fd70f16f4b1

#include <type_traits>
#include <string>

#include "Vector.hpp"

namespace LibRay::Math
{
class Ray final
{
public:
	Ray(Vector3 const &origin, Vector3 const &direction);

	Vector3 const &Origin() const;
	Vector3 const &Direction() const;

	std::string ToString() const;

private:
	Vector3 origin, direction;
};

static_assert(std::is_copy_constructible_v<Ray>);
static_assert(std::is_copy_assignable_v<Ray>);
static_assert(std::is_trivially_copyable_v<Ray>);

static_assert(std::is_move_constructible_v<Ray>);
static_assert(std::is_move_assignable_v<Ray>);
} // namespace LibRay::Math

#endif // fa606def_a0a5_4c2c_8152_2fd70f16f4b1
