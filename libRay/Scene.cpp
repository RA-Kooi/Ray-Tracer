#include "Scene.hpp"

#include <cassert>

#include "Material/Material.hpp"
#include "Math/Vector.hpp"
#include "Math/MathUtils.hpp"
#include "Shaders/BlinnPhong.hpp"
#include "Shaders/Lambertian.hpp"
#include "Shapes/Triangle.hpp"
#include "Utilites.hpp"

namespace LibRay
{
using namespace Materials;
using namespace Math;
using namespace Shapes;

Scene::Scene(
	class Camera&& camera,
	std::uint64_t,
	Color const &ambientLight,
	float ambientIntensity)
: camera(std::move(camera))
, shapes()
, unboundableShapes()
, bvh()
, lights()
, ambientLight(ambientLight)
, ambientIntensity(ambientIntensity)
, shaderStore()
, materialStore()
{
	Stopwatch watch;
	watch.Start();

	lights.emplace_back(Vector3(0.f, 24.f, 0.f), Color::White(), 300.0f);
	lights.emplace_back(Vector3(0.f, 0.f, 50.f), Color::White(), 1000.0f);

	Shader const &blinnPhong = shaderStore.AddShader(
		"Blinn-Phong",
		std::make_unique<BlinnPhongShader>());

	//constexpr float const eggshell = 10;
	constexpr float const mildlyShiny = 100;
	//constexpr float const shiny = 1000;
	//constexpr float const veryShiny = 10'000;

	Material material(blinnPhong, 0.f);
	material.UpdateTextureProperty("diffuse", Texture::Blue());
	material.UpdateColorProperty("specular", Color::White());
	material.UpdateFloatProperty("phong exponent", mildlyShiny);

	MaterialStore::IndexType const blue =
		materialStore.AddMaterial(std::move(material));

	auto triangle = std::make_unique<Triangle>(
		Transform(Vector3(0), Vector3(0, 0, Math::PI * 0.5f), Vector3(5)),
		materialStore,
		blue);

	shapes.push_back(std::move(triangle));

	watch.Stop();

	std::printf(
		"Loading scene took %s to place %zu objects\n",
		watch.Value().c_str(),
		shapes.size());
	std::fflush(stdout);

	watch.Start();

	std::vector<Observer<BaseShape<Shape> const>> shapesForBVH;
	shapesForBVH.reserve(shapes.size());

	for(std::unique_ptr<Shape> &shape: shapes)
		shape->Transform().RecalculateMatrix();

	for(std::unique_ptr<Shape> const &shape: shapes)
	{
		if(shape->IsBoundable())
			shapesForBVH.push_back(shape.get());
		else
			unboundableShapes.push_back(shape.get());
	}

	bvh = std::make_unique<Containers::BVH<Shape>>(std::move(shapesForBVH));

	watch.Stop();

	std::printf("BVH creation took %s\n", watch.Value().c_str());
	std::fflush(stdout);
}

Camera const &Scene::Camera() const
{
	return camera;
}

std::vector<Observer<Shape const>> const &Scene::UnboundableShapes() const
{
	return unboundableShapes;
}

std::vector<std::unique_ptr<Shape>> const &Scene::Shapes() const
{
	return shapes;
}

Containers::BVH<Shape> const &Scene::BoundingVolumeHierarchy() const
{
	assert(bvh);

	return *bvh;
}

std::vector<Light> const &Scene::Lights() const
{
	return lights;
}

std::pair<Materials::Color const &, float> Scene::AmbientLight() const
{
	return {ambientLight, ambientIntensity};
}
} // namespace LibRay
