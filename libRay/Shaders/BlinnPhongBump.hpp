#ifndef ea7adef6_423d_3d46_762e_6df16a6b78e3
#define ea7adef6_423d_3d46_762e_6df16a6b78e3

#include <type_traits>
#include <vector>

#include "../Math/Vector.hpp"
#include "../Utilites.hpp"
#include "Shader.hpp"

namespace LibRay
{
namespace Math
{
class Ray;
} // namespace Math

namespace Materials
{
class Color;
} // namespace Materials

class Intersection;
class Light;

class BlinnPhongShaderBump: public Shader
{
public:
	Materials::Color Run(
		Intersection const &intersection,
		Math::Vector3 const &cameraPosition,
		Math::Ray const &ray,
		std::vector<Observer<Light const>> const &lights,
		Materials::Color const &ambientLight,
		float ambientIntensity) const override;
};

static_assert(std::is_copy_constructible_v<BlinnPhongShaderBump>);
static_assert(std::is_copy_assignable_v<BlinnPhongShaderBump>);
static_assert(!std::is_trivially_copyable_v<BlinnPhongShaderBump>);

static_assert(std::is_move_constructible_v<BlinnPhongShaderBump>);
static_assert(std::is_move_assignable_v<BlinnPhongShaderBump>);
} // namespace LibRay

#endif // ea7adef6_423d_3d46_762e_6df16a6b78e3
