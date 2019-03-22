#include "ColorOnly.hpp"

#include "../Shapes/Color.hpp"
#include "../Shapes/Shape.hpp"
#include "../Intersection.hpp"

namespace LibRay
{
using namespace Math;
using namespace Shapes;

Shapes::Color ColorOnlyShader::Run(
	Intersection const &intersection,
	Vector3 const &,
	Ray const &,
	std::vector<Observer<Light const>> const &,
	Color const &,
	float) const
{
	return intersection.shape->Color();
}
} // namespace LibRay
