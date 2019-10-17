#include "Scene.hpp"

#include <cassert>

#include "Material/Material.hpp"
#include "Math/Vector.hpp"
#include "Math/MathUtils.hpp"
#include "Shaders/BlinnPhong.hpp"
#include "Shaders/BlinnPhongBump.hpp"
#include "Shaders/EnvironmentMapping.hpp"
#include "Shapes/Model/Model.hpp"
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
, modelLoader(materialStore)
{
	Stopwatch watch;
	watch.Start();

	lights.emplace_back(Vector3(0.f, 24.f, 0.f), Color::White(), 300.0f);
	lights.emplace_back(Vector3(0.f, 0.f, 50.f), Color::White(), 1000.0f);

	Shader const &blinnPhong = shaderStore.AddShader(
		"Blinn-Phong",
		std::make_unique<BlinnPhongShader>());

	Shader const &envMap = shaderStore.AddShader(
		"Environment Mapping",
		std::make_unique<EnvironmentMappingShader>());

	//constexpr float const eggshell = 10;
	constexpr float const mildlyShiny = 100;
	//constexpr float const shiny = 1000;
	//constexpr float const veryShiny = 10'000;

	Texture planeTex("Resources/Textures/seamless_tileable_grass.jpg");

	Material material(blinnPhong, 0.001f, 0.f);
	material.UpdateFloatProperty("phong exponent", mildlyShiny);
	material.UpdateTextureProperty("diffuse", planeTex);
	material.UpdateTextureProperty("specular", Texture::White());

	MaterialStore::IndexType const planeMat =
		materialStore.AddMaterial("Plane", material);

	material.RefractiveIndexInside(1.5f);
	material.Absorbtion(Color(0.6f, 0.4f, 0.1f));

	MaterialStore::IndexType const sphereMat =
		materialStore.AddMaterial("Sphere", material);

	Material material3(envMap);
	Texture envMapTex("Resources/Textures/blue_grotto_4k.hdr");
	material3.UpdateTextureProperty("diffuse", envMapTex);

	MaterialStore::IndexType const envMapMat =
		materialStore.AddMaterial("Environment Map", std::move(material3));

	auto plane = std::make_unique<Plane>(
		Transform(Vector3(0, -10, 0)),
		materialStore,
		planeMat);

	shapes.push_back(std::move(plane));

	auto environment = std::make_unique<Sphere>(
		Transform(camera.Transform().Position(), Vector3(0, Math::PI * 0.5f, 0), Vector3(100)),
		materialStore,
		envMapMat);

	shapes.push_back(std::move(environment));

	auto sphere = std::make_unique<Sphere>(
		Transform(Vector3(0, -5, -10), Vector3(0), Vector3(10)),
		materialStore,
		sphereMat);

	shapes.push_back(std::move(sphere));

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

void Scene::LoadModel(
	std::string const &fileName,
	Transform const &transform,
	MaterialStore::IndexType materialIndex,
	bool invertNormalZ)
{
	std::vector<std::unique_ptr<Model>> models = modelLoader.LoadObj(
		fileName,
		transform,
		"Resources/Materials",
		materialIndex,
		invertNormalZ);

	shapes.insert(
		shapes.end(),
		std::make_move_iterator(models.begin()),
		std::make_move_iterator(models.end()));
}
} // namespace LibRay
