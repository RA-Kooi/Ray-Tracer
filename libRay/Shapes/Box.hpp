#ifndef a0b9169e_9c51_d49d_6fc7_c2b2fb5f32da
#define a0b9169e_9c51_d49d_6fc7_c2b2fb5f32da

#include <boost/optional.hpp>
#include <type_traits>

#include "../Shaders/MaterialStore.hpp"
#include "../API.hpp"
#include "Shape.hpp"

namespace LibRay
{
namespace Containers
{
class BoundingBox;
} // namespace Containers

namespace Math
{
class Ray;
} // namespace Math

class Intersection;

namespace Shapes
{
class LIBRAY_API Box final: public Shape
{
public:
	using Shape::Shape;

	boost::optional<Intersection> Intersects(Math::Ray const &ray) const override;

	Containers::BoundingBox CalculateBoundingBox() const override;
};

static_assert(std::is_copy_constructible_v<Box>);
static_assert(!std::is_copy_assignable_v<Box>);
static_assert(!std::is_trivially_copyable_v<Box>);

static_assert(std::is_move_constructible_v<Box>);
static_assert(!std::is_move_assignable_v<Box>);
} // namespace Shapes
} // namespace LibRay

#endif // a0b9169e_9c51_d49d_6fc7_c2b2fb5f32da
