#include "Color.hpp"

#include "../Math/MathUtils.hpp"

namespace LibRay::Shapes
{
Color::Color(float red, float green, float blue)
: r(red)
, g(green)
, b(blue)
{
}

Color Color::operator+() const
{
	return Color(+r, +g, +b);
}

Color Color::operator-() const
{
	return Color(-r, -g, -b);
}

Color &Color::operator+=(Color const &other)
{
	*this = *this + other;
	return *this;
}

Color &Color::operator-=(Color const &other)
{
	*this = *this - other;
	return *this;
}

Color &Color::operator*=(Color const &other)
{
	*this = *this * other;
	return *this;
}

Color &Color::operator/=(Color const &other)
{
	*this = *this / other;
	return *this;
}

Color &Color::operator*=(float value)
{
	*this = *this * value;
	return *this;
}

Color &Color::operator/=(float value)
{
	*this = *this / value;
	return *this;
}

Color &Color::Clamp()
{
	r = Math::Clamp(r, 0.0f, 1.0f);
	g = Math::Clamp(g, 0.0f, 1.0f);
	b = Math::Clamp(b, 0.0f, 1.0f);

	return *this;
}

Color Color::Clamped() const
{
	Color copy = *this;
	return copy.Clamp();
}

std::string Color::ToString() const
{
	return "["
		"r: " + std::to_string(r)
		+ ", g: " + std::to_string(g)
		+ ", b: " + std::to_string(b)
		+ "]";
}

Color operator+(Color const &a, Color const &b)
{
	return Color(a.r + b.r, a.g + b.g, a.b + b.b);
}

Color operator-(Color const &a, Color const &b)
{
	return a + -b;
}

Color operator*(Color const &a, Color const &b)
{
	return Color(a.r * b.r, a.g * b.g, a.b * b.b);
}

Color operator/(Color const &a, Color const &b)
{
	return Color(a.r / b.r, a.g / b.g, a.b / b.b);
}

Color operator*(Color const &a, float value)
{
	return Color(a.r * value, a.g * value, a.b * value);
}

Color operator*(float value, Color const &b)
{
	return b * value;
}

Color operator/(Color const &a, float value)
{
	return Color(a.r / value, a.g / value, a.b / value);
}

Color Color::Red()
{
	return Color(1, 0, 0);
}

Color Color::Green()
{
	return Color(0, 1, 0);
}

Color Color::Blue()
{
	return Color(0, 0, 1);
}

Color Color::Black()
{
	return Color(0, 0, 0);
}

Color Color::White()
{
	return Color(1, 1, 1);
}
} // namespace LibRay::Shapes
