#include "Lambertian.hpp"

#include <algorithm>

#include "../Material/Color.hpp"
#include "../Math/MathUtils.hpp"
#include "../Shapes/Shape.hpp"
#include "../Intersection.hpp"
#include "../Light.hpp"

namespace LibRay
{
using namespace Math;
using namespace Shapes;

Color LambertianShader::Run(
	Intersection const &intersection,
	Vector3 const &,
	Ray const &,
	std::vector<Observer<Light const>> const &lights,
	Shapes::Color const &ambientLight,
	float ambientIntensity) const
{
	Color result = ambientLight * ambientIntensity;
	Shape const &shape = *intersection.shape;

	Vector3 const &intersectionPos = intersection.worldPosition;

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
			* shape.Color();
	}

	return result;
}
} // namespace LibRay
