#include "Ray.hpp"

namespace LibRay::Math
{
Ray::Ray(Vector3 const &origin, Vector3 const &direction)
: origin(origin)
, direction(direction.normalized())
{
}

Vector3 const &Ray::Origin() const
{
	return origin;
}

Vector3 const &Ray::Direction() const
{
	return direction;
}
} // namespace LibRay::Math
