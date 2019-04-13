#include "Vector3.hpp"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdio>
#include <string>

namespace LibRay::Math
{
Vector3::Vector3()
: x(0)
, y(0)
, z(0)
{
}

Vector3::Vector3(float x, float y, float z)
: x(x)
, y(y)
, z(z)
{
}

Vector3 Vector3::operator+() const
{
	return *this;
}

Vector3 Vector3::operator-() const
{
	return Vector3(-x, -y, -z);
}

Vector3 &Vector3::operator+=(Vector3 const &other)
{
	*this = *this + other;

	return *this;
}

Vector3 &Vector3::operator-=(Vector3 const &other)
{
	*this = *this - other;

	return *this;
}

Vector3 &Vector3::operator*=(float value)
{
	*this = *this * value;

	return *this;
}

Vector3 &Vector3::operator/=(float value)
{
	*this = *this / value;

	return *this;
}

Vector3 &Vector3::operator*=(Vector3 const &other)
{
	*this = *this * other;

	return *this;
}

Vector3 &Vector3::operator/=(Vector3 const &other)
{
	*this = *this / other;

	return *this;
}

float Vector3::operator[](int index) const
{
	assert(index < 3 && index >= 0);

	return v[index];
}

float &Vector3::operator[](int index)
{
	assert(index < 3 && index >= 0);

	return v[index];
}

Vector3 &Vector3::Normalize()
{
	*this /= Magnitude();

	return *this;
}

Vector3 Vector3::Normalized() const
{
	Vector3 copy = *this;
	copy.Normalize();
	return copy;
}

float Vector3::Dot(Vector3 const &other) const
{
	return x * other.x + y * other.y + z * other.z;
}

Vector3 Vector3::Cross(Vector3 const &other) const
{
	float const cx = y * other.z - z * other.y;
	float const cy = z * other.x - x * other.z;
	float const cz = x * other.y - y * other.x;

	return Vector3(cx, cy, cz);
}

float Vector3::SquareMagnitude() const
{
	return x * x + y * y + z * z;
}

float Vector3::Magnitude() const
{
	return std::sqrt(SquareMagnitude());
}

Vector3 &Vector3::Abs()
{
	x = std::abs(x);
	y = std::abs(y);
	z = std::abs(z);

	return *this;
}

Vector3 Vector3::Abs() const
{
	Vector3 copy = *this;
	return copy.Abs();
}

float Vector3::MaxComponent() const
{
	return std::max(x, std::max(y, z));
}

float Vector3::MinComponent() const
{
	return std::min(x, std::min(y, z));
}

Vector3 Vector3::Min(Vector3 const &other) const
{
	return Vector3(
		std::min(x, other.x),
		std::min(y, other.y),
		std::min(z, other.z));
}

Vector3 Vector3::Max(Vector3 const &other) const
{
	return Vector3(
		std::max(x, other.x),
		std::max(y, other.y),
		std::max(z, other.z));
}

void Vector3::Print() const
{
	std::string const output
		= "[x: " + std::to_string(x)
		+ ", y: " + std::to_string(y)
		+ ", z: " + std::to_string(z)
		+ "]\n";

	std::puts(output.c_str());
}

Vector3 operator+(Vector3 const &lhs, Vector3 const &rhs)
{
	return Vector3(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z);
}

Vector3 operator-(Vector3 const &lhs, Vector3 const &rhs)
{
	return Vector3(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z);
}

Vector3 operator*(Vector3 const &lhs, float value)
{
	return Vector3(lhs.x * value, lhs.y * value, lhs.z * value);
}

Vector3 operator*(float value, Vector3 const &rhs)
{
	return rhs * value;
}

Vector3 operator/(Vector3 const &lhs, float value)
{
	return Vector3(lhs.x / value, lhs.y / value, lhs.z / value);
}

Vector3 operator*(Vector3 const &lhs, Vector3 const &rhs)
{
	return Vector3(lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z);
}

Vector3 operator/(float value, Vector3 const &rhs)
{
	return Vector3(value / rhs.x, value / rhs.y, value / rhs.z);
}

Vector3 operator/(Vector3 const &lhs, Vector3 const &rhs)
{
	return Vector3(lhs.x / rhs.x, lhs.y / rhs.y, lhs.z / rhs.z);
}

bool operator==(Vector3 const &lhs, Vector3 const &rhs)
{
	bool equal = true;

	for(int i = 0; i < 3; ++i)
	{
		if((lhs[i] - rhs[i]) != 0.f)
		{
			equal = false;
			break;
		}
	}

	return equal;
}

bool operator!=(Vector3 const &lhs, Vector3 const &rhs)
{
	return !(lhs == rhs);
}
} // namespace LibRay::Math
