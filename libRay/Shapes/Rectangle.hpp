#ifndef e9b78d4d_f3d8_3424_ba11_26f7f71e0bfc
#define e9b78d4d_f3d8_3424_ba11_26f7f71e0bfc

#include <memory>
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
class LIBRAY_API Rectangle final: public Shape
{
public:
	using Shape::Shape;

	virtual ~Rectangle() noexcept override = default;

	std::optional<Intersection> IntersectsInternal(
		Math::Ray const &ray) const override;

	Containers::BoundingBox CalculateBoundingBoxInternal() const override;
};

static_assert(std::is_copy_constructible_v<Rectangle>);
static_assert(!std::is_copy_assignable_v<Rectangle>);
static_assert(!std::is_trivially_copyable_v<Rectangle>);

static_assert(std::is_move_constructible_v<Rectangle>);
static_assert(!std::is_move_assignable_v<Rectangle>);
} // namespace Shapes
} // namespace LibRay

#endif // e9b78d4d_f3d8_3424_ba11_26f7f71e0bfc
