#include "BlinnPhongBump.hpp"

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

Color BlinnPhongShaderBump::Run(
	Intersection const &intersection,
	Vector3 const &view,
	Ray const &,
	std::vector<Observer<Light const>> const &lights,
	Color const &ambientLight,
	float const ambientIntensity) const
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

	Vector3 const normal = glm::normalize(
		baseNormal
		+ (bumpUV - bumpUMinusOne) * tangent
		+ (bumpUV - bumpVMinusOne) * biTangent);

	Vector3 const &intersectionPos = intersection.worldPosition;
	Color const &diffuse =
		material.TexturePropertyByName("diffuse").Sample(uv.x, uv.y);

	Color const &baseSpecular =
		material.TexturePropertyByName("specular").Sample(uv.x, uv.y);
	float const phongExponent = material.FloatPropertyByName("phong exponent");

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
