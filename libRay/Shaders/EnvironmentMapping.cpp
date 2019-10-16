#include "EnvironmentMapping.hpp"

#include "../Material/Color.hpp"
#include "../Shapes/Shape.hpp"
#include "../Intersection.hpp"

namespace LibRay
{
using namespace Materials;
using namespace Math;

Color EnvironmentMappingShader::Run(
	Intersection const &intersection,
	Math::Vector3 const &,
	Math::Ray const &,
	std::vector<Observer<Light const>> const &,
	Materials::Color const &,
	float) const
{
	Vector2 const &uv = intersection.uv;
	Material const &material = intersection.shape->Material();
	Texture const &diffuse = material.TexturePropertyByName("diffuse");

	return diffuse.Sample(uv.x, uv.y);
}
} // namespace LibRay
