#ifndef fdaf49f0_4ed4_2adc_90ba_f583a61830c7
#define fdaf49f0_4ed4_2adc_90ba_f583a61830c7

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

class NormalVizShader final: public Shader
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

static_assert(std::is_copy_constructible_v<NormalVizShader>);
static_assert(std::is_copy_assignable_v<NormalVizShader>);
static_assert(!std::is_trivially_copyable_v<NormalVizShader>);

static_assert(std::is_move_constructible_v<NormalVizShader>);
static_assert(std::is_move_assignable_v<NormalVizShader>);
} // namespace LibRay

#endif // fdaf49f0_4ed4_2adc_90ba_f583a61830c7
