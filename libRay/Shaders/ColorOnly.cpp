#include "ColorOnly.hpp"

#include "../Material/Color.hpp"
#include "../Shapes/Shape.hpp"
#include "../Intersection.hpp"

namespace LibRay
{
using namespace Materials;
using namespace Math;

Color ColorOnlyShader::Run(
	Intersection const &intersection,
	Vector3 const &,
	Ray const &,
	std::vector<Observer<Light const>> const &,
	Color const &,
	float) const
{
	return intersection.shape->Material().ColorPropertyByName("Color");
}
} // namespace LibRay
