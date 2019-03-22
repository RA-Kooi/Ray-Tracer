#include "Vector2.hpp"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdio>
#include <string>

#include <Windows.h>

namespace LibRay::Math
{
Vector2::Vector2()
: x(0)
, y(0)
{
}

Vector2::Vector2(float x, float y)
: x(x)
, y(y)
{
}

Vector2 Vector2::operator+() const
{
	return *this;
}

Vector2 Vector2::operator-() const
{
	return Vector2(-x, -y);
}

Vector2 &Vector2::operator+=(Vector2 const &other)
{
	*this = *this + other;

	return *this;
}

Vector2 &Vector2::operator-=(Vector2 const &other)
{
	*this = *this - other;

	return *this;
}

Vector2 &Vector2::operator*=(float value)
{
	*this = *this * value;

	return *this;
}

Vector2 &Vector2::operator/=(float value)
{
	*this = *this / value;

	return *this;
}

Vector2 &Vector2::operator*=(Vector2 const &other)
{
	*this = *this * other;

	return *this;
}

Vector2 &Vector2::operator/=(Vector2 const &other)
{
	*this = *this / other;

	return *this;
}

float Vector2::operator[](int index) const
{
	assert(index == 0 || index == 1);

	return v[index];
}

float &Vector2::operator[](int index)
{
	assert(index == 0 || index == 1);

	return v[index];
}

Vector2 &Vector2::Normalize()
{
	*this /= Magnitude();

	return *this;
}

Vector2 Vector2::Normalized() const
{
	Vector2 copy = *this;
	copy.Normalize();
	return copy;
}

float Vector2::Dot(Vector2 const &other) const
{
	return x * other.x + y * other.y;
}

float Vector2::SquareMagnitude() const
{
	return x * x + y * y;
}

float Vector2::Magnitude() const
{
	return std::sqrt(SquareMagnitude());
}

Vector2 &Vector2::Abs()
{
	x = std::abs(x);
	y = std::abs(y);

	return *this;
}

Vector2 Vector2::Abs() const
{
	Vector2 copy = *this;
	return copy.Abs();
}

float Vector2::MaxComponent() const
{
	return std::max(x, y);
}

float Vector2::MinComponent() const
{
	return std::min(x, y);
}

Vector2 Vector2::Min(Vector2 const &other) const
{
	return Vector2(std::min(x, other.x), std::min(y, other.y));
}

Vector2 Vector2::Max(Vector2 const &other) const
{
	return Vector2(std::max(x, other.x), std::max(y, other.y));
}

void Vector2::Print() const
{
	std::string const output
		= "[x: " + std::to_string(x)
		+ ", y: " + std::to_string(y)
		+ "]\n";

	std::puts(output.c_str());

	OutputDebugStringA(output.c_str());
}

Vector2 operator+(Vector2 const &lhs, Vector2 const &rhs)
{
	return Vector2(lhs.x + rhs.x, lhs.y + rhs.y);
}

Vector2 operator-(Vector2 const &lhs, Vector2 const &rhs)
{
	return Vector2(lhs.x - rhs.x, lhs.y - rhs.y);
}

Vector2 operator*(Vector2 const &lhs, float value)
{
	return Vector2(lhs.x * value, lhs.y * value);
}

Vector2 operator*(float value, Vector2 const &rhs)
{
	return rhs * value;
}

Vector2 operator/(Vector2 const &lhs, float value)
{
	return Vector2(lhs.x / value, lhs.y / value);
}

Vector2 operator*(Vector2 const &lhs, Vector2 const &rhs)
{
	return Vector2(lhs.x * rhs.x, lhs.y * rhs.y);
}

Vector2 operator/(float value, Vector2 const &rhs)
{
	return Vector2(value / rhs.x, value / rhs.y);
}

Vector2 operator/(Vector2 const &lhs, Vector2 const &rhs)
{
	return Vector2(lhs.x / rhs.x, lhs.y / rhs.y);
}

bool operator==(Vector2 const &lhs, Vector2 const &rhs)
{
	if((lhs.x - rhs.x) != 0.f)
		return false;

	if((lhs.y - rhs.y) != 0.f)
		return false;

	return true;
}

bool operator!=(Vector2 const &lhs, Vector2 const &rhs)
{
	return !(lhs == rhs);
}
} // namespace LibRay::Math
