#ifndef a07ce4c6_4023_939e_c4e9_36960fba34c1
#define a07ce4c6_4023_939e_c4e9_36960fba34c1

#include <optional>
#include <type_traits>

#include "../Math/Vector.hpp"
#include "../Material/MaterialStore.hpp"
#include "../API.hpp"
#include "../Transform.hpp"
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
class LIBRAY_API Disc final: public Shape
{
public:
	using Shape::Shape;

	virtual ~Disc() noexcept override = default;

	std::optional<Intersection> IntersectsInternal(
		Math::Ray const &ray) const override;

	Containers::BoundingBox CalculateBoundingBoxInternal() const override;
};

static_assert(std::is_copy_constructible_v<Disc>);
static_assert(!std::is_copy_assignable_v<Disc>);
static_assert(!std::is_trivially_copyable_v<Disc>);

static_assert(std::is_move_constructible_v<Disc>);
static_assert(!std::is_move_assignable_v<Disc>);
} // namespace Shapes
} // namespace LibRay


#endif // a07ce4c6_4023_939e_c4e9_36960fba34c1
