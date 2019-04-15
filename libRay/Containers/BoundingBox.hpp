#ifndef b0715ebf_925f_a3e3_29b7_252e6b9fe589
#define b0715ebf_925f_a3e3_29b7_252e6b9fe589

#include <type_traits>

#include "../Math/Vector.hpp"
#include "../API.hpp"

namespace LibRay
{
namespace Math
{
class Ray;
} // namespace Math

namespace Containers
{
class LIBRAY_API BoundingBox
{
public:
	BoundingBox(
		Math::Vector3 const &dimensions,
		Math::Vector3 const &position);

	float Intersects(Math::Ray const &ray) const;

	Math::Vector3 const &Dimensions() const;
	Math::Vector3 const &Position() const;

private:
	Math::Vector3 dimensions, position;
};

static_assert(std::is_copy_constructible_v<BoundingBox>);
static_assert(std::is_copy_assignable_v<BoundingBox>);
static_assert(std::is_trivially_copyable_v<BoundingBox>);

static_assert(std::is_move_constructible_v<BoundingBox>);
static_assert(std::is_move_assignable_v<BoundingBox>);
} // namespace Containers
} // namespace LibRay

#endif // b0715ebf_925f_a3e3_29b7_252e6b9fe589
