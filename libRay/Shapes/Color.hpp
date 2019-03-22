#ifndef b42831bc_5aec_4ee2_9bdd_09f0dac7b019
#define b42831bc_5aec_4ee2_9bdd_09f0dac7b019

#include <type_traits>

#include "../API.hpp"

namespace LibRay::Shapes
{
class LIBRAY_API Color final
{
public:
	Color(float red, float green, float blue);

	Color operator+() const;
	Color operator-() const;

	Color &operator+=(Color const &other);
	Color &operator-=(Color const &other);
	Color &operator*=(Color const &other);
	Color &operator/=(Color const &other);

	Color &operator*=(float value);
	Color &operator/=(float value);

	Color &Clamp();
	Color Clamped() const;

	static Color Red();
	static Color Green();
	static Color Blue();
	static Color Black();
	static Color White();

public:
	float r, g, b;
};

LIBRAY_API Color operator+(Color const &a, Color const &b);
LIBRAY_API Color operator-(Color const &a, Color const &b);
LIBRAY_API Color operator*(Color const &a, Color const &b);
LIBRAY_API Color operator/(Color const &a, Color const &b);

LIBRAY_API Color operator*(Color const &a, float value);
LIBRAY_API Color operator*(float value, Color const &b);
LIBRAY_API Color operator/(Color const &a, float value);

static_assert(std::is_copy_constructible_v<Color>);
static_assert(std::is_copy_assignable_v<Color>);
static_assert(std::is_trivially_copyable_v<Color>);

static_assert(std::is_move_constructible_v<Color>);
static_assert(std::is_move_assignable_v<Color>);
} // namespace LibRay::Shapes

#endif // b42831bc_5aec_4ee2_9bdd_09f0dac7b019
