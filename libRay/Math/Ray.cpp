#include "Ray.hpp"

namespace LibRay::Math
{
Ray::Ray(Vector3 origin, Vector3 direction)
: origin(origin)
, direction(direction.Normalize())
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
