#include "Scene.hpp"

#include <cassert>

#include "Material/Material.hpp"
#include "Math/Vector.hpp"
#include "Math/MathUtils.hpp"
#include "Shaders/BlinnPhong.hpp"
#include "Shaders/Lambertian.hpp"
#include "Shapes/Box.hpp"
#include "Shapes/Plane.hpp"
#include "Shapes/Sphere.hpp"
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

	Shader const &lambertian = shaderStore.AddShader(
		"Lambertian",
		std::make_unique<LambertianShader>());

	Texture checkers("./Resources/Textures/1024x1024 Texel Density Texture 1.png");
	Texture earth("./Resources/Textures/2k_earth_daymap.jpg");
	Texture container("./Resources/Textures/container.jpg");

	//constexpr float const eggshell = 10;
	constexpr float const mildlyShiny = 100;
	//constexpr float const shiny = 1000;
	//constexpr float const veryShiny = 10'000;

	Material material(lambertian, 0.f);
	material.UpdateTextureProperty("diffuse", Texture::White());
	//material.UpdateColorProperty("specular", Color::White());
	//material.UpdateFloatProperty("phong exponent", eggshell);

	MaterialStore::IndexType const topBack =
		materialStore.AddMaterial(material);

	material.UpdateTextureProperty("diffuse", checkers);
	MaterialStore::IndexType const bottomMat =
		materialStore.AddMaterial(material);

	material.UpdateTextureProperty("diffuse", Texture::Green());
	MaterialStore::IndexType const leftMat =
		materialStore.AddMaterial(material);

	material.UpdateTextureProperty("diffuse", Texture::Red());
	MaterialStore::IndexType const rightMat =
		materialStore.AddMaterial(material);

	material.UpdateTextureProperty("diffuse", Texture::Blue());
	MaterialStore::IndexType const blueBox =
		materialStore.AddMaterial(material);

	material.UpdateTextureProperty("diffuse", container);
	MaterialStore::IndexType const containerBox =
		materialStore.AddMaterial(std::move(material));

	Material material2(blinnPhong, 0.05f);
	material2.UpdateTextureProperty("diffuse", earth);
	material2.UpdateColorProperty("specular", Color::White());
	material2.UpdateFloatProperty("phong exponent", mildlyShiny);
	MaterialStore::IndexType const blueSphere =
		materialStore.AddMaterial(material2);

	material2.RefractiveIndex(1.49f);
	MaterialStore::IndexType const refractiveSphere =
		materialStore.AddMaterial(std::move(material2));

	auto box = std::make_unique<Box>(
		Transform(Vector3(-9.f, -21.f, 0.f), Vector3(0), Vector3(8, 8, 8)),
		materialStore,
		containerBox,
		Color::Green() * 0.7f + Color::White() * 0.3f);

	shapes.push_back(std::move(box));

	box = std::make_unique<Box>(
		Transform(Vector3(9.f, -15.f, 0.f), Vector3(0, Math::PI * 0.25f, 0), Vector3(8, 20, 8)),
		materialStore,
		blueBox,
		Color::Blue() * 0.7f + Color::White() * 0.3f);

	shapes.push_back(std::move(box));

	auto sphere = std::make_unique<Sphere>(
		Transform(
			Vector3(3.f, 0.f, 0.f),
			Vector3(-Math::PI * 0.2f, -Math::PI * 0.4f, 0),
			Vector3(3.f)),
		materialStore,
		blueSphere,
		Color::Blue());

	shapes.push_back(std::move(sphere));

	auto refracSphere = std::make_unique<Sphere>(
		Transform(
			Vector3(0.f, 0.f, 15.f),
			Vector3(0),
			Vector3(5.f)),
		materialStore,
		refractiveSphere,
		Color::White());

	shapes.push_back(std::move(refracSphere));

	auto bottom = std::make_unique<Plane>(
		Transform(Vector3(0, -25, 0)),
		materialStore,
		bottomMat,
		Color::White());

	shapes.push_back(std::move(bottom));

	auto back = std::make_unique<Plane>(
		Transform(Vector3(0, 0, -25), Vector3(Math::PI * 0.5f, 0, 0)),
		materialStore,
		topBack,
		Color::White());

	shapes.push_back(std::move(back));

	auto top = std::make_unique<Plane>(
		Transform(Vector3(0, 25, 0), Vector3(Math::PI, 0, 0)),
		materialStore,
		topBack,
		Color::White());

	shapes.push_back(std::move(top));

	auto right = std::make_unique<Plane>(
		Transform(Vector3(25, 0, 0), Vector3(0, 0, Math::PI * 0.5f)),
		materialStore,
		rightMat,
		Color::Green());

	shapes.push_back(std::move(right));

	auto left = std::make_unique<Plane>(
		Transform(Vector3(-25, 0, 0), Vector3(0, 0, -Math::PI * 0.5f)),
		materialStore,
		leftMat,
		Color::Red());

	shapes.push_back(std::move(left));

	watch.Stop();

	std::printf(
		"Loading scene took %s to place %zu objects\n",
		watch.Value().c_str(),
		shapes.size());

	std::vector<Observer<Shape const>> shapesForBVH;
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

	bvh = std::make_unique<Containers::BVH>(std::move(shapesForBVH));
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

Containers::BVH const &Scene::BoundingVolumeHierarchy() const
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
