#ifndef f4ffcf96_123c_4559_9031_f794398a0fe5
#define f4ffcf96_123c_4559_9031_f794398a0fe5

#include <optional>
#include <type_traits>

#include "../Containers/BoundingBox.hpp"
#include "../Material/MaterialStore.hpp"
#include "../Math/Vector.hpp"
#include "../API.hpp"
#include "../Intersection.hpp"
#include "../Transform.hpp"

namespace LibRay
{
namespace Math
{
class Ray;
} // namespace Math

namespace Shapes
{
template<typename T>
class BaseShape
{
	inline T const &Derived() const;

public:
	BaseShape(BaseShape const &other) = default;
	BaseShape(BaseShape &&other) = default;
	~BaseShape() noexcept;

	BaseShape &operator=(BaseShape const &other) = default;
	BaseShape &operator=(BaseShape &&other) = default;

	Math::Vector3 const Position() const;

	inline bool IsBoundable() const;

	inline std::optional<Intersection> Intersects(Math::Ray const &ray) const;

	inline Containers::BoundingBox CalculateBoundingBox() const;

private:
	BaseShape() = default;

	friend T;
};

class LIBRAY_API Shape: public BaseShape<Shape>
{
public:
	Shape(
		class Transform const &transform,
		Materials::MaterialStore const &materialStore,
		Materials::MaterialStore::IndexType materialIndex);

	virtual ~Shape() noexcept;

	Materials::Material const &Material() const;

	class Transform const &Transform() const;
	class Transform &Transform();

	virtual bool IsBoundableInternal() const;

	virtual std::optional<Intersection> IntersectsInternal(
		Math::Ray const &ray) const = 0;

	virtual Containers::BoundingBox CalculateBoundingBoxInternal() const = 0;

private:
	Math::Vector3 const PositionInternal() const;
	friend class BaseShape<Shape>;

protected:
	class Transform transform;
	Materials::MaterialStore const &materialStore;
	Materials::MaterialStore::IndexType materialIndex;
};

static_assert(!std::is_copy_constructible_v<Shape>);
static_assert(!std::is_copy_assignable_v<Shape>);
static_assert(!std::is_trivially_copyable_v<Shape>);

static_assert(!std::is_move_constructible_v<Shape>);
static_assert(!std::is_move_assignable_v<Shape>);

static_assert(std::is_copy_constructible_v<BaseShape<Shape>>);
static_assert(std::is_copy_assignable_v<BaseShape<Shape>>);
static_assert(!std::is_trivially_copyable_v<BaseShape<Shape>>);

static_assert(std::is_move_constructible_v<BaseShape<Shape>>);
static_assert(std::is_move_assignable_v<BaseShape<Shape>>);
} // namespace Shapes
} // namespace LibRay


#if !defined(VIM_WORKAROUND)
#include "Shape_impl.hpp"
#endif

#endif // f4ffcf96_123c_4559_9031_f794398a0fe5
