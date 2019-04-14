#include "Intersection.hpp"

namespace LibRay
{
using namespace Math;
using namespace Shapes;

Intersection::Intersection(
	Shape const &shape,
	Vector3 const &surfaceNormal,
	Vector3 const &worldPosition)
: shape(&shape)
, surfaceNormal(surfaceNormal.normalized())
, worldPosition(worldPosition)
{
}
} // namespace LibRay
