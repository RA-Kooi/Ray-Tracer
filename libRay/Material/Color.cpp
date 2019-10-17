#include "Color.hpp"

#include "../Math/MathUtils.hpp"

namespace LibRay::Materials
{
Color::Color(float red, float green, float blue)
: r(red)
, g(green)
, b(blue)
{
}

Color::Color(Math::Vector3 const &v)
: r(v.r)
, g(v.g)
, b(v.b)
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

Color::operator Math::Vector3() const
{
	return Math::Vector3(r, g, b);
}

Color &Color::operator=(Math::Vector3 const &other)
{
	r = other.r;
	g = other.g;
	b = other.b;

	return *this;
}

bool Color::operator==(Color const &other) const
{
	return (r - other.r) == 0.f && (g - other.g) == 0.f && (b - other.b) == 0.f;
}

bool Color::operator!=(Color const &other) const
{
	return !(*this == other);
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

Color operator+(Color const &a, float value)
{
	return Color(a.r + value, a.g + value, a.b + value);
}

Color operator+(float value, Color const &b)
{
	return b + value;
}

Color operator-(Color const &a, float value)
{
	return Color(a.r - value, a.g - value, a.b - value);
}

Color operator-(float value, Color const &b)
{
	return Color(value - b.r, value - b.g, value - b.b);
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

Color operator/(float value, Color const &b)
{
	return Color(value / b.r, value / b.g, value / b.b);
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
} // namespace LibRay::Materials

std::ostream &operator<<(std::ostream &o, LibRay::Materials::Color const &c)
{
	return o << "[R: " << c.r << ", G: " << c.g << ", B: " << c.b << "]";
}
