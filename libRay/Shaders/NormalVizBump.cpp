#include "NormalVizBump.hpp"

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

Color NormalVizShaderBump::Run(
	Intersection const &intersection,
	Vector3 const &,
	Ray const &,
	std::vector<Observer<Light const>> const &,
	Color const &,
	float) const
{
	Shape const &shape = *intersection.shape;
	Material const &material = shape.Material();

	// N'u,v = N + (Bu,v - Bu-1,v)t + (Bu,v - Bu,v-1)b;

	Vector3 const &baseNormal = intersection.surfaceNormal;
	Vector3 const &tangent = intersection.surfaceTangent;
	Vector3 const biTangent = glm::cross(baseNormal, tangent);
	Vector2 const &uv = intersection.uv;

	Texture const &bumpMap = material.TexturePropertyByName("bump map");
	Vector3 const bumpUV = bumpMap.Sample(uv.x, uv.y);
	Vector3 const bumpUMinusOne = bumpMap.Sample(uv.x - 1.f, uv.y);
	Vector3 const bumpVMinusOne = bumpMap.Sample(uv.x, uv.y - 1.f);
	float const bumpStrength = material.FloatPropertyByName("bump strength");

	Vector3 const normal = glm::normalize(
		baseNormal * bumpStrength
		+ (bumpUV - bumpUMinusOne) * tangent
		+ (bumpUV - bumpVMinusOne) * biTangent);

	Vector3 const col = (0.5f * normal) + Vector3(0.5f);

	return Color(col.x, col.y, col.z);
}
} // namespace LibRay
