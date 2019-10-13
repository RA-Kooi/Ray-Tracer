#ifndef b0731c79_ae35_830e_9bd9_1c6eba4e3426
#define b0731c79_ae35_830e_9bd9_1c6eba4e3426

#include <cstdint>
#include <memory>
#include <type_traits>
#include <utility>
#include <vector>

#include "Containers/BoundingVolumeHierarchy.hpp"
#include "Material/MaterialStore.hpp"
#include "Shaders/ShaderStore.hpp"
#include "Shapes/Model/ModelLoader.hpp"
#include "API.hpp"
#include "Camera.hpp"
#include "Light.hpp"

namespace LibRay
{
namespace Shapes
{
class Shape;
} // namespace Shapes

class Transform;

class LIBRAY_API Scene final
{
public:
	Scene(
		Camera&& camera,
		std::uint64_t seed,
		Materials::Color const &ambientLight,
		float ambientIntensity);

	Scene(Scene &&other) = default;
	Scene(Scene const &) = delete;

	Scene &operator=(Scene &&other) = delete;
	Scene &operator=(Scene const &) = delete;

	Camera const &Camera() const;

	std::vector<Observer<Shapes::Shape const>> const &UnboundableShapes() const;
	std::vector<std::unique_ptr<Shapes::Shape>> const &Shapes() const;
	Containers::BVH<Shapes::Shape> const &BoundingVolumeHierarchy() const;

	std::vector<Light> const &Lights() const;

	std::pair<Materials::Color const &, float> AmbientLight() const;

private:
	void LoadModel(
		std::string const &fileName,
		Transform const &transform,
		Materials::MaterialStore::IndexType materialIndex = 0,
		bool invertNormalZ = false);

private:
	class Camera camera;

	std::vector<std::unique_ptr<Shapes::Shape>> shapes;
	std::vector<Observer<Shapes::Shape const>> unboundableShapes;
	std::unique_ptr<Containers::BVH<Shapes::Shape>> bvh;

	std::vector<Light> lights;

	Materials::Color ambientLight;
	float ambientIntensity;

	ShaderStore shaderStore;
	Materials::MaterialStore materialStore;

	Shapes::ModelLoader modelLoader;
};

static_assert(!std::is_copy_constructible_v<Scene>);
static_assert(!std::is_copy_assignable_v<Scene>);
static_assert(!std::is_trivially_copyable_v<Scene>);

static_assert(std::is_move_constructible_v<Scene>);
static_assert(!std::is_move_assignable_v<Scene>);
} // namespace LibRay

#endif // b0731c79_ae35_830e_9bd9_1c6eba4e3426
