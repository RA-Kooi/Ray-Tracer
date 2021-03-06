#ifndef f2086a7b_1753_4654_8390_7cf9da1085d9
#define f2086a7b_1753_4654_8390_7cf9da1085d9

#include <type_traits>
#include <vector>

#include "../Math/Vector.hpp"
#include "../Utilites.hpp"

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

class Shader
{
public:
	virtual ~Shader() noexcept;

	virtual Materials::Color Run(
		Intersection const &intersection,
		Math::Vector3 const &view,
		Math::Ray const &ray,
		std::vector<Observer<Light const>> const &lights,
		Materials::Color const &ambientLight,
		float ambientIntensity) const = 0;
};

static_assert(!std::is_copy_constructible_v<Shader>);
static_assert(std::is_copy_assignable_v<Shader>);
static_assert(!std::is_trivially_copyable_v<Shader>);

static_assert(!std::is_move_constructible_v<Shader>);
static_assert(std::is_move_assignable_v<Shader>);
} // namespace LibRay

#endif // f2086a7b_1753_4654_8390_7cf9da1085d9
