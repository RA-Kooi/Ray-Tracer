#ifndef e2b92e5d_bd94_a706_372e_8f0fd4e6e502
#define e2b92e5d_bd94_a706_372e_8f0fd4e6e502

#include <type_traits>
#include <vector>

#include "../Math/Vector.hpp"
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

class ColorOnlyShader final: public Shader
{
public:
	virtual ~ColorOnlyShader() noexcept override = default;

	Materials::Color Run(
		Intersection const &intersection,
		Math::Vector3 const &cameraPosition,
		Math::Ray const &ray,
		std::vector<Observer<Light const>> const &lights,
		Materials::Color const &ambientLight,
		float ambientIntensity) const override;
};

static_assert(std::is_copy_constructible_v<ColorOnlyShader>);
static_assert(std::is_copy_assignable_v<ColorOnlyShader>);
static_assert(!std::is_trivially_copyable_v<ColorOnlyShader>);

static_assert(std::is_move_constructible_v<ColorOnlyShader>);
static_assert(std::is_move_assignable_v<ColorOnlyShader>);
} // namespace LibRay

#endif // e2b92e5d_bd94_a706_372e_8f0fd4e6e502
