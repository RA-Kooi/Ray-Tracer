#ifndef b0731c79_ae35_830e_9bd9_1c6eba4e3426
#define b0731c79_ae35_830e_9bd9_1c6eba4e3426

#include <cstdint>
#include <memory>
#include <type_traits>
#include <utility>
#include <vector>

#include "Containers/BoundingVolumeHierarchy.hpp"
#include "Shaders/MaterialStore.hpp"
#include "Shaders/ShaderStore.hpp"
#include "API.hpp"
#include "Camera.hpp"
#include "Light.hpp"

namespace LibRay
{
namespace Shapes
{
class Shape;
} // namespace Shapes

class LIBRAY_API Scene final
{
public:
	Scene(
		Camera&& camera,
		std::uint64_t seed,
		Shapes::Color const &ambientLight,
		float ambientIntensity);

	Scene(Scene &&other) = default;
	Scene(Scene const &) = delete;

	Scene &operator=(Scene &&other) = default;
	Scene &operator=(Scene const &) = delete;

	Camera const &Camera() const;

	std::vector<Observer<Shapes::Shape const>> const &UnboundableShapes() const;
	std::vector<std::unique_ptr<Shapes::Shape>> const &Shapes() const;
	Containers::BVH const &BoundingVolumeHierarchy() const;

	std::vector<Light> const &Lights() const;

	std::pair<Shapes::Color const &, float> AmbientLight() const;

private:
	class Camera camera;

	std::vector<std::unique_ptr<Shapes::Shape>> shapes;
	std::vector<Observer<Shapes::Shape const>> unboundableShapes;
	std::unique_ptr<Containers::BVH> bvh;

	std::vector<Light> lights;

	Shapes::Color ambientLight;
	float ambientIntensity;

	ShaderStore shaderStore;
	MaterialStore materialStore;
};

static_assert(!std::is_copy_constructible_v<Scene>);
static_assert(!std::is_copy_assignable_v<Scene>);
static_assert(!std::is_trivially_copyable_v<Scene>);

static_assert(std::is_move_constructible_v<Scene>);
static_assert(std::is_move_assignable_v<Scene>);
} // namespace LibRay

#endif // b0731c79_ae35_830e_9bd9_1c6eba4e3426