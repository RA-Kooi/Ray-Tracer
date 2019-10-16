#include "Scene.hpp"

#include <cassert>

#include "Material/Material.hpp"
#include "Math/Vector.hpp"
#include "Math/MathUtils.hpp"
#include "Shaders/BlinnPhong.hpp"
#include "Shaders/BlinnPhongBump.hpp"
#include "Shapes/Model/Model.hpp"
#include "Shapes/Plane.hpp"
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

	Shader const &blinnPhongBump = shaderStore.AddShader(
		"Blinn-Phong Bump",
		std::make_unique<BlinnPhongShaderBump>());

	//constexpr float const eggshell = 10;
	constexpr float const mildlyShiny = 100;
	//constexpr float const shiny = 1000;
	//constexpr float const veryShiny = 10'000;

	Texture planeTex("Resources/Textures/1024x1024 Texel Density Texture 1.png");

	Material material(blinnPhong, 0.1f, 0.f);
	material.UpdateFloatProperty("phong exponent", mildlyShiny);
	material.UpdateTextureProperty("diffuse", planeTex);
	material.UpdateTextureProperty("specular", Texture::White());

	MaterialStore::IndexType const planeMat =
		materialStore.AddMaterial("Plane", std::move(material));

	Material material2(blinnPhongBump, 0.02f, 0.f);
	material2.UpdateColorProperty("specular", Color::White());
	material2.UpdateFloatProperty("phong exponent", mildlyShiny);
	material2.UpdateFloatProperty("bump strength", 0.5f);

	Texture armTex("Resources/Textures/NanoSuit/arm_dif.png");
	material2.UpdateTextureProperty("diffuse", armTex);
	Texture armBumpTex("Resources/Textures/NanoSuit/arm_showroom_ddn.png");
	material2.UpdateTextureProperty("bump map", armBumpTex);
	Texture armSpecTex("Resources/Textures/NanoSuit/arm_showroom_spec.png");
	material2.UpdateTextureProperty("specular", armSpecTex);
	materialStore.AddMaterial("Arm", material2);

	Texture bodyTex("Resources/Textures/NanoSuit/body_dif.png");
	material2.UpdateTextureProperty("diffuse", bodyTex);
	Texture bodyBumpTex("Resources/Textures/NanoSuit/body_showroom_ddn.png");
	material2.UpdateTextureProperty("bump map", bodyBumpTex);
	Texture bodySpecTex("Resources/Textures/NanoSuit/body_showroom_spec.png");
	material2.UpdateTextureProperty("specular", bodySpecTex);
	materialStore.AddMaterial("Body", material2);

	Texture glassTex("Resources/Textures/NanoSuit/glass_dif.png");
	material2.UpdateTextureProperty("diffuse", glassTex);
	Texture glassBumpTex("Resources/Textures/NanoSuit/glass_ddn.png");
	material2.UpdateTextureProperty("bump map", glassBumpTex);
	material2.UpdateTextureProperty("specular", Texture::White());
	materialStore.AddMaterial("Glass", material2);

	Texture handTex("Resources/Textures/NanoSuit/hand_dif.png");
	material2.UpdateTextureProperty("diffuse", handTex);
	Texture handBumpTex("Resources/Textures/NanoSuit/hand_showroom_ddn.png");
	material2.UpdateTextureProperty("bump map", handBumpTex);
	Texture handSpecTex("Resources/Textures/NanoSuit/hand_showroom_spec.png");
	material2.UpdateTextureProperty("specular", handSpecTex);
	materialStore.AddMaterial("Hand", material2);

	Texture helmetTex("Resources/Textures/NanoSuit/helmet_dif.png");
	material2.UpdateTextureProperty("diffuse", helmetTex);
	Texture helmetBumpTex("Resources/Textures/NanoSuit/helmet_showroom_ddn.png");
	material2.UpdateTextureProperty("bump map", helmetBumpTex);
	Texture helmetSpecTex("Resources/Textures/NanoSuit/helmet_showroom_spec.png");
	material2.UpdateTextureProperty("specular", helmetSpecTex);
	materialStore.AddMaterial("Helmet", material2);

	Texture legTex("Resources/Textures/NanoSuit/leg_dif.png");
	material2.UpdateTextureProperty("diffuse", legTex);
	Texture legBumpTex("Resources/Textures/NanoSuit/leg_showroom_ddn.png");
	material2.UpdateTextureProperty("bump map", legBumpTex);
	Texture legSpecTex("Resources/Textures/NanoSuit/leg_showroom_spec.png");
	material2.UpdateTextureProperty("specular", legSpecTex);
	materialStore.AddMaterial("Leg", material2);

	LoadModel(
		"Resources/Models/nanosuit.obj",
		Transform(Vector3(0, -10, 0), Vector3(0), Vector3(1)));

	auto plane = std::make_unique<Plane>(
		Transform(Vector3(0, -10, 0)),
		materialStore,
		planeMat);

	shapes.push_back(std::move(plane));

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
