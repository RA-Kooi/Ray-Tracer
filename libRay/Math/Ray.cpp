#include "Ray.hpp"

namespace LibRay::Math
{
Ray::Ray(Vector3 const &origin, Vector3 const &direction)
: origin(origin)
, direction(glm::normalize(direction))
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

std::string Ray::ToString() const
{
	return "Origin: ["
		"x: " + std::to_string(origin.x)
		+ ", y: " + std::to_string(origin.y)
		+ ", z: " + std::to_string(origin.z)
		+ "], "
		+ "Direction: ["
		"x: " + std::to_string(direction.x)
		+ ", y: " + std::to_string(direction.y)
		+ ", z: " + std::to_string(direction.z)
		+ "]";
}
} // namespace LibRay::Math
