#ifndef f2aeb659_a16c_d5ed_5750_31618073bc21
#define f2aeb659_a16c_d5ed_5750_31618073bc21

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

class NormalVizShaderBump final: public Shader
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

static_assert(std::is_copy_constructible_v<NormalVizShaderBump>);
static_assert(std::is_copy_assignable_v<NormalVizShaderBump>);
static_assert(!std::is_trivially_copyable_v<NormalVizShaderBump>);

static_assert(std::is_move_constructible_v<NormalVizShaderBump>);
static_assert(std::is_move_assignable_v<NormalVizShaderBump>);
} // namespace LibRay

#endif // f2aeb659_a16c_d5ed_5750_31618073bc21
