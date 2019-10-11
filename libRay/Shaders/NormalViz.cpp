#include "NormalViz.hpp"

#include <algorithm>

#include "../Material/Color.hpp"
#include "../Math/MathUtils.hpp"
#include "../Shapes/Shape.hpp"
#include "../Intersection.hpp"
#include "../Light.hpp"

namespace LibRay
{
using namespace Materials;
using namespace Math;
using namespace Shapes;

Color NormalVizShader::Run(
	Intersection const &intersection,
	Vector3 const &,
	Ray const &,
	std::vector<Observer<Light const>> const &,
	Color const &,
	float) const
{
	Vector3 const col = (0.5f * intersection.surfaceNormal) + Vector3(0.5f);

	return Color(col.x, col.y, col.z);
}
} // namespace LibRay
