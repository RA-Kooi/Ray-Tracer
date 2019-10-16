#ifndef bcea0efa_919d_a594_808c_5859178a482d
#define bcea0efa_919d_a594_808c_5859178a482d

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

class EnvironmentMappingShader: public Shader
{
public:
	virtual ~EnvironmentMappingShader() noexcept override = default;

	Materials::Color Run(
		Intersection const &intersection,
		Math::Vector3 const &cameraPosition,
		Math::Ray const &ray,
		std::vector<Observer<Light const>> const &lights,
		Materials::Color const &ambientLight,
		float ambientIntensity) const override;
};

static_assert(std::is_copy_constructible_v<EnvironmentMappingShader>);
static_assert(std::is_copy_assignable_v<EnvironmentMappingShader>);
static_assert(!std::is_trivially_copyable_v<EnvironmentMappingShader>);

static_assert(std::is_move_constructible_v<EnvironmentMappingShader>);
static_assert(std::is_move_assignable_v<EnvironmentMappingShader>);
} // namespace LibRay

#endif // bcea0efa_919d_a594_808c_5859178a482d
