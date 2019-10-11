#ifndef f4ffcf96_123c_4559_9031_f794398a0fe5
#define f4ffcf96_123c_4559_9031_f794398a0fe5

#include <optional>
#include <type_traits>

#include "../Material/MaterialStore.hpp"
#include "../Math/Vector.hpp"
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
class ModelTriangle;

template<typename T>
class LIBRAY_API BaseShape
{
public:
	BaseShape(BaseShape const &other) = default;
	BaseShape(BaseShape &&other) = default;
	virtual ~BaseShape() noexcept;

	BaseShape &operator=(BaseShape const &other) = default;
	BaseShape &operator=(BaseShape &&other) = default;

	Math::Vector3 const Position() const;

	virtual bool IsBoundable() const;

	virtual std::optional<Intersection> Intersects(
		Math::Ray const &ray) const = 0;

	virtual Containers::BoundingBox CalculateBoundingBox() const = 0;

private:
	BaseShape() = default;

	friend T;
};

class LIBRAY_API Shape: public BaseShape<Shape>
{
public:
	Shape(
		Transform const &transform,
		Materials::MaterialStore const &materialStore,
		Materials::MaterialStore::IndexType materialIndex);

	virtual ~Shape() noexcept;

	Materials::Material const &Material() const;

	Transform const &Transform() const;
	class Transform &Transform();

private:
	Math::Vector3 const PositionInternal() const;
	friend class BaseShape<Shape>;

protected:
	class Transform transform;
	Materials::MaterialStore const &materialStore;
	Materials::MaterialStore::IndexType materialIndex;
};

extern template class BaseShape<Shape>;
extern template class BaseShape<ModelTriangle>;

static_assert(!std::is_copy_constructible_v<Shape>);
static_assert(!std::is_copy_assignable_v<Shape>);
static_assert(!std::is_trivially_copyable_v<Shape>);

static_assert(!std::is_move_constructible_v<Shape>);
static_assert(!std::is_move_assignable_v<Shape>);

static_assert(!std::is_copy_constructible_v<BaseShape<Shape>>);
static_assert(std::is_copy_assignable_v<BaseShape<Shape>>);
static_assert(!std::is_trivially_copyable_v<BaseShape<Shape>>);

static_assert(!std::is_move_constructible_v<BaseShape<Shape>>);
static_assert(std::is_move_assignable_v<BaseShape<Shape>>);
} // namespace Shapes
} // namespace LibRay

#endif // f4ffcf96_123c_4559_9031_f794398a0fe5
