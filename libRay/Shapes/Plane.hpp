#ifndef e4b156b9_390a_71bc_51c8_27e7ece591c3
#define e4b156b9_390a_71bc_51c8_27e7ece591c3

#include <optional>
#include <type_traits>

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
class LIBRAY_API Plane final: public Shape
{
public:
	using Shape::Shape;

	virtual ~Plane() noexcept override = default;

	std::optional<Intersection> Intersects(Math::Ray const &ray) const override;

	bool IsBoundable() const override;
	Containers::BoundingBox CalculateBoundingBox() const override;
};

static_assert(std::is_copy_constructible_v<Plane>);
static_assert(!std::is_copy_assignable_v<Plane>);
static_assert(!std::is_trivially_copyable_v<Plane>);

static_assert(std::is_move_constructible_v<Plane>);
static_assert(!std::is_move_assignable_v<Plane>);
} // namespace Shapes
} // namespace LibRay

#endif // e4b156b9_390a_71bc_51c8_27e7ece591c3
