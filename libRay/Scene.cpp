#include "Scene.hpp"

#include <cassert>

#include "Math/Vector.hpp"
#include "Shaders/BlinnPhong.hpp"
#include "Shaders/Material.hpp"
#include "Shapes/Box.hpp"
#include "Shapes/Plane.hpp"
#include "Shapes/Sphere.hpp"
#include "Utilites.hpp"

namespace LibRay
{
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

	lights.emplace_back(Vector3(-20.f, 20.f, -20.f), Color::Red(), 1000.0f);
	lights.emplace_back(Vector3(0.f, 40.f, 0.f), Color::White(), 2000.0f);

	Shader const &blinnPhong = shaderStore.AddShader(
		"Blinn-Phong",
		std::make_unique<BlinnPhongShader>());

	constexpr float const eggshell = 10;
	constexpr float const mildlyShiny = 100;
	//constexpr float const shiny = 1000;
	//constexpr float const veryShiny = 10'000;

	Material material(blinnPhong, 0.5f);
	material.UpdateColorProperty("specular", Color::White());
	material.UpdateFloatProperty("phong exponent", eggshell);

	Material material2(material);
	material2.UpdateFloatProperty("phong exponent", mildlyShiny);
	material2.Reflectiveness(0.5f);

	MaterialStore::IndexType const &blinnPhongMaterial =
		materialStore.AddMaterial(std::move(material));

	MaterialStore::IndexType const &mildlyShinyMaterial =
		materialStore.AddMaterial(std::move(material2));

	auto plane = std::make_unique<Plane>(
		Transform(Vector3(0, -10, 0)),
		materialStore,
		blinnPhongMaterial,
		Color::White() * 0.7f);

	shapes.push_back(std::move(plane));

	bool placeBox = true;

	for(int x = -200; x <= 200; x += 20)
	{
		for(int z = -380; z <= 20; z += 20)
		{
			if(placeBox)
			{
				auto box = std::make_unique<Box>(
					Transform(
						Vector3(float(x), 0, float(z)),
						Vector3(0),
						Vector3(10, 20, 10)),
					materialStore,
					mildlyShinyMaterial,
					(Color::Blue() + Color::White()) * 0.5f);

				shapes.push_back(std::move(box));
			}
			else
			{
				auto sphere = std::make_unique<Sphere>(
					5.f,
					Transform(Vector3(float(x), 0, float(z))),
					materialStore,
					mildlyShinyMaterial,
					Color::White() * 0.85f);

				shapes.push_back(std::move(sphere));
			}

			placeBox = !placeBox;
		}
	}

	watch.Stop();

	std::printf(
		"Loading scene took %s to place %zu objects\n",
		watch.Value().c_str(),
		shapes.size());

	std::vector<Observer<Shape const>> shapesForBVH;
	shapesForBVH.reserve(shapes.size());

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

std::pair<Shapes::Color const &, float> Scene::AmbientLight() const
{
	return {ambientLight, ambientIntensity};
}
} // namespace LibRay
