#ifndef f4ffcf96_123c_4559_9031_f794398a0fe5
#define f4ffcf96_123c_4559_9031_f794398a0fe5

#include <optional>
#include <type_traits>

#include "../Material/Color.hpp"
#include "../Material/MaterialStore.hpp"
#include "../API.hpp"
#include "../Transform.hpp"

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
class LIBRAY_API Shape
{
public:
	Shape(
		Transform const &transform,
		Materials::MaterialStore const &materialStore,
		Materials::MaterialStore::IndexType materialIndex,
		Materials::Color const &color);

	virtual ~Shape() noexcept = default;

	Materials::Color const &Color() const;
	Materials::Material const &Material() const;
	Transform const &Transform() const;
	class Transform &Transform();

	virtual bool IsBoundable() const;

	virtual std::optional<Intersection> Intersects(
		Math::Ray const &ray) const = 0;

	virtual Containers::BoundingBox CalculateBoundingBox() const = 0;

protected:
	Materials::Color color;
	Materials::MaterialStore const &materialStore;
	Materials::MaterialStore::IndexType materialIndex;

	class Transform transform;
};

static_assert(!std::is_copy_constructible_v<Shape>);
static_assert(!std::is_copy_assignable_v<Shape>);
static_assert(!std::is_trivially_copyable_v<Shape>);

static_assert(!std::is_move_constructible_v<Shape>);
static_assert(!std::is_move_assignable_v<Shape>);
} // namespace Shapes
} // namespace LibRay

#endif // f4ffcf96_123c_4559_9031_f794398a0fe5
