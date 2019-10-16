#include "Intersection.hpp"

namespace LibRay
{
using namespace Math;
using namespace Shapes;

Intersection::Intersection(
	Shape const &shape,
	Vector3 const &surfaceNormal,
	Vector3 const &worldPosition,
	Vector2 const &uv)
: shape(&shape)
, surfaceNormal(glm::normalize(surfaceNormal))
, surfaceTangent()
, worldPosition(worldPosition)
, uv(uv)
{
}

Intersection::Intersection(
	Shape const &shape,
	Vector3 const &surfaceNormal,
	Vector3 const &surfaceTangent,
	Vector3 const &worldPosition,
	Vector2 const &uv)
: shape(&shape)
, surfaceNormal(glm::normalize(surfaceNormal))
, surfaceTangent(glm::normalize(surfaceTangent))
, worldPosition(worldPosition)
, uv(uv)
{
}
} // namespace LibRay
