#ifndef ef3c24db_f4e3_cb3c_8c82_de05c3264cb5
#define ef3c24db_f4e3_cb3c_8c82_de05c3264cb5

#include <cstdint>
#include <string>
#include <type_traits>
#include <vector>

#include "../Math/Vector.hpp"
#include "Color.hpp"

namespace LibRay::Materials
{
class Texture final
{
public:
	enum class WrappingMethod
	{
		Repeat,
		Clamp
	};

	enum class InputFormat: std::size_t
	{
		GRAYSCALE = 1,
		RGB = 3,
		RGBA = 4
	};

public:
	Texture(
		std::string const &fileName,
		WrappingMethod wrapMethodU = WrappingMethod::Repeat,
		WrappingMethod wrapMethodV = WrappingMethod::Repeat);

	Texture(
		Math::Vector2st const &dimensions,
		std::uint8_t const *const rawData,
		InputFormat inputFormat,
		WrappingMethod wrapMethodU = WrappingMethod::Repeat,
		WrappingMethod wrapMethodV = WrappingMethod::Repeat);

	Texture(
		Math::Vector2st const &dimensions,
		std::vector<Color> rgbData,
		WrappingMethod wrapMethodU = WrappingMethod::Repeat,
		WrappingMethod wrapMethodV = WrappingMethod::Repeat);

	void WrapMethodU(WrappingMethod newMethod);
	WrappingMethod WrapMethodU() const;

	void WrapMethodV(WrappingMethod newMethod);
	WrappingMethod WrapMethodV() const;

	Color const &Sample(float u, float v) const;

	// Built-in 1x1 textures
	static Texture const &Black();
	static Texture const &Blue();
	static Texture const &Green();
	static Texture const &Red();
	static Texture const &White();

private:
	Math::Vector2st dimensions;
	std::vector<Color> rgbData;
	WrappingMethod wrapMethodU, wrapMethodV;
};

static_assert(std::is_copy_constructible_v<Texture>);
static_assert(std::is_copy_assignable_v<Texture>);
static_assert(!std::is_trivially_copyable_v<Texture>);

static_assert(std::is_move_constructible_v<Texture>);
static_assert(std::is_move_assignable_v<Texture>);
} // namespace LibRay

#endif // ef3c24db_f4e3_cb3c_8c82_de05c3264cb5
