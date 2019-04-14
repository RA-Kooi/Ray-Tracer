#ifndef d652ec07_dbb8_87b6_f1c0_d527b2da78b0
#define d652ec07_dbb8_87b6_f1c0_d527b2da78b0

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

namespace Shapes
{
class Color;
} // namespace Shapes

class Intersection;
class Light;

class LambertianShader final: public Shader
{
public:
	Shapes::Color Run(
		Intersection const &intersection,
		Math::Vector3 const &cameraPosition,
		Math::Ray const &ray,
		std::vector<Observer<Light const>> const &lights,
		Shapes::Color const &ambientLight,
		float ambientIntensity) const override;
};

static_assert(std::is_copy_constructible_v<LambertianShader>);
static_assert(std::is_copy_assignable_v<LambertianShader>);
static_assert(!std::is_trivially_copyable_v<LambertianShader>);

static_assert(std::is_move_constructible_v<LambertianShader>);
static_assert(std::is_move_assignable_v<LambertianShader>);
} // namespace LibRay

#endif // d652ec07_dbb8_87b6_f1c0_d527b2da78b0
