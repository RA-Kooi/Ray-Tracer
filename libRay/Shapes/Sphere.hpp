#ifndef b00f71ed_c38b_4932_8e9f_d6fdb6700f2a
#define b00f71ed_c38b_4932_8e9f_d6fdb6700f2a

#include <optional>
#include <type_traits>

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
class LIBRAY_API Sphere final: public Shape
{
public:
	Sphere(
		float radius,
		class Transform const &transform,
		Materials::MaterialStore const &materialStore,
		Materials::MaterialStore::IndexType materialIndex,
		Materials::Color const &color);

	virtual ~Sphere() noexcept override = default;

	std::optional<Intersection> Intersects(Math::Ray const &ray) const override;

	Containers::BoundingBox CalculateBoundingBox() const override;

private:
	float radius;
};

static_assert(std::is_copy_constructible_v<Sphere>);
static_assert(!std::is_copy_assignable_v<Sphere>);
static_assert(!std::is_trivially_copyable_v<Sphere>);

static_assert(std::is_move_constructible_v<Sphere>);
static_assert(!std::is_move_assignable_v<Sphere>);
} // namespace Shapes
} // namespace LibRay

#endif // b00f71ed_c38b_4932_8e9f_d6fdb6700f2a
