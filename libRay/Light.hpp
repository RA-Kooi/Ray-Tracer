#ifndef f74e8b15_e159_603a_db85_9b8f5662e576
#define f74e8b15_e159_603a_db85_9b8f5662e576

#include <type_traits>

#include "Material/Color.hpp"
#include "Math/Vector.hpp"
#include "API.hpp"

namespace LibRay
{
class LIBRAY_API Light final
{
public:
	Light(
		Math::Vector3 const &position,
		Materials::Color const &color,
		float intensity);

	Math::Vector3 const &Position() const;
	Materials::Color const &Color() const;
	float Intensity() const;

private:
	Math::Vector3 position;
	Materials::Color color;
	float intensity;
};

static_assert(std::is_copy_constructible_v<Light>);
static_assert(std::is_copy_assignable_v<Light>);
static_assert(std::is_trivially_copyable_v<Light>);

static_assert(std::is_move_constructible_v<Light>);
static_assert(std::is_move_assignable_v<Light>);
} // namespace LibRay

#endif // f74e8b15_e159_603a_db85_9b8f5662e576
