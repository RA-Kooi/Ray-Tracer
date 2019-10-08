#include "Lambertian.hpp"

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

Color LambertianShader::Run(
	Intersection const &intersection,
	Vector3 const &,
	Ray const &,
	std::vector<Observer<Light const>> const &lights,
	Color const &ambientLight,
	float ambientIntensity) const
{
	Color result = ambientLight * ambientIntensity;
	Shape const &shape = *intersection.shape;
	Material const &material = shape.Material();

	Vector3 const &intersectionPos = intersection.worldPosition;
	Vector2 const &uv = intersection.uv;
	Color const &diffuse =
		material.TexturePropertyByName("diffuse").Sample(uv.x, uv.y);

	for(Observer<Light const> const &light: lights)
	{
		Vector3 const intersectionToLight = light->Position() - intersectionPos;

		float const intersectionToLightDistance = glm::length(intersectionToLight);

		Vector3 const intersectionToLightDirection =
			intersectionToLight / intersectionToLightDistance;

		float const costheta =
			glm::dot(intersection.surfaceNormal, intersectionToLightDirection);

		float const r2 = intersectionToLightDistance * intersectionToLightDistance;
		float const I = light->Intensity() / r2;
		result += std::max(costheta, 0.f)
			* light->Color()
			* I
			* diffuse;
	}

	return result;
}
} // namespace LibRay
