#include "BlinnPhong.hpp"

#include <algorithm>
#include <cmath>

#include "../Material/Color.hpp"
#include "../Material/Material.hpp"
#include "../Math/Ray.hpp"
#include "../Shapes/Shape.hpp"
#include "../Intersection.hpp"
#include "../Light.hpp"

namespace LibRay
{
using namespace Materials;
using namespace Math;
using namespace Shapes;

Color BlinnPhongShader::Run(
	Intersection const &intersection,
	Vector3 const &view,
	Ray const &,
	std::vector<Observer<Light const>> const &lights,
	Color const &ambientLight,
	float const ambientIntensity) const
{
	Shape const &shape = *intersection.shape;
	Material const &material = shape.Material();
	float const phongExponent = material.FloatPropertyByName("phong exponent");
	Color const &baseSpecular = material.ColorPropertyByName("specular");

	Vector3 const &normal = intersection.surfaceNormal;
	Vector3 const &intersectionPos = intersection.worldPosition;
	Color const &diffuse = shape.Color();

	Color result = ambientLight * ambientIntensity;

	for(Observer<Light const> const &light: lights)
	{
		Vector3 const intersectionToLight = light->Position() - intersectionPos;

		float const intersectionToLightDistance =
			glm::length(intersectionToLight);

		Vector3 const intersectionToLightDirection =
			intersectionToLight / intersectionToLightDistance;

		float const cosTheta = glm::dot(normal, intersectionToLightDirection);

		Vector3 const viewLightBisector =
			glm::normalize(view + intersectionToLightDirection);

		float const viewLightDirection = glm::dot(normal, viewLightBisector);

		float const r2 = intersectionToLightDistance * intersectionToLightDistance;
		float const I = light->Intensity() / r2;
		Color const lambertianPart = diffuse
			* light->Color()
			* I
			* std::max(0.f, cosTheta);

		Color const specular = baseSpecular * light->Color();
		Color const blinnPhongPart = specular
			* I
			* std::pow(std::max(0.f, viewLightDirection), phongExponent);

		result += lambertianPart + blinnPhongPart;
	}

	return result;
}
} // namespace LibRay
