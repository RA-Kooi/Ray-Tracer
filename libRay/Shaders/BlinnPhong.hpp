#ifndef b083649e_c489_56d3_56f5_8179d0784371
#define b083649e_c489_56d3_56f5_8179d0784371

#include <type_traits>
#include <vector>

#include "../Utilites.hpp"
#include "Shader.hpp"

namespace LibRay
{
namespace Math
{
class Ray;
class Vector3;
} // namespace Math

namespace Shapes
{
class Color;
} // namespace Shapes

class Intersection;
class Light;

class BlinnPhongShader: public Shader
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

static_assert(std::is_copy_constructible_v<BlinnPhongShader>);
static_assert(std::is_copy_assignable_v<BlinnPhongShader>);
static_assert(!std::is_trivially_copyable_v<BlinnPhongShader>);

static_assert(std::is_move_constructible_v<BlinnPhongShader>);
static_assert(std::is_move_assignable_v<BlinnPhongShader>);
} // namespace LibRay

#endif // b083649e_c489_56d3_56f5_8179d0784371
