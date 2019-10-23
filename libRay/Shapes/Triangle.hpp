#ifndef bde1d03a_205c_5b41_3d9e_ae6b7e9fc950
#define bde1d03a_205c_5b41_3d9e_ae6b7e9fc950

#include <optional>
#include <type_traits>

#include "../Material/MaterialStore.hpp"
#include "../API.hpp"
#include "Shape.hpp"

namespace LibRay
{
namespace Math
{
class Ray;
} // namespace  Math

class Intersection;

namespace Shapes
{
class LIBRAY_API Triangle final: public Shape
{
public:
	using Shape::Shape;

	std::optional<Intersection> IntersectsInternal(
		Math::Ray const &ray) const override;

	Containers::BoundingBox CalculateBoundingBoxInternal() const override;
};

static_assert(std::is_copy_constructible_v<Triangle>);
static_assert(!std::is_copy_assignable_v<Triangle>);
static_assert(!std::is_trivially_copyable_v<Triangle>);

static_assert(std::is_move_constructible_v<Triangle>);
static_assert(!std::is_move_assignable_v<Triangle>);
} // namespace Shapes
} // namespace LibRay

#endif // bde1d03a_205c_5b41_3d9e_ae6b7e9fc950
