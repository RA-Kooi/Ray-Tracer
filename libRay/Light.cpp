#include "Light.hpp"

using namespace LibRay::Materials;
using namespace LibRay::Math;

namespace LibRay
{
Light::Light(
	Math::Vector3 const &position,
	class Color const &color,
	float intensity)
: position(position)
, color(color)
, intensity(intensity)
{
}

Vector3 const &Light::Position() const
{
	return position;
}

Color const &Light::Color() const
{
	return color;
}

float Light::Intensity() const
{
	return intensity;
}
} // namespace LibRay
