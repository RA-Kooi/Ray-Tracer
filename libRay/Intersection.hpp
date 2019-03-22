#ifndef d21b0db1_f84d_4356_8923_2a8b8a0a7f6b
#define d21b0db1_f84d_4356_8923_2a8b8a0a7f6b

#include <type_traits>

#include "Math/Vector3.hpp"
#include "API.hpp"
#include "Utilites.hpp"

namespace LibRay
{
namespace Shapes
{
class Shape;
} // namespace Shapes

class LIBRAY_API Intersection final
{
public:
	Intersection(
		Shapes::Shape const &shape,
		Math::Vector3 const &surfaceNormal,
		Math::Vector3 const &worldPosition);

public:
	Observer<Shapes::Shape const> shape;
	Math::Vector3 surfaceNormal;
	Math::Vector3 worldPosition;
};

static_assert(std::is_copy_constructible_v<Intersection>);
static_assert(std::is_copy_assignable_v<Intersection>);
static_assert(std::is_trivially_copyable_v<Intersection>);

static_assert(std::is_move_constructible_v<Intersection>);
static_assert(std::is_move_assignable_v<Intersection>);
} // namespace LibRay

#endif // d21b0db1_f84d_4356_8923_2a8b8a0a7f6b
