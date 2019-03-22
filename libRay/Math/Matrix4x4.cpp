#include "Matrix4x4.hpp"

#include <array>
#include <cassert>
#include <cstdio>
#include <cstring>
#include <cmath>
#include <string>

#include <Windows.h>
#undef far
#undef near

namespace LibRay::Math
{
Matrix4x4::Matrix4x4()
{
	std::memset(m, 0, 16 * sizeof(float));
}

Matrix4x4::Matrix4x4(
	Vector3 row0, float r03,
	Vector3 row1, float r13,
	Vector3 row2, float r23,
	Vector3 row3, float r33)
: r00(row0.x), r01(row0.y), r02(row0.z), r03(r03)
, r10(row1.x), r11(row1.y), r12(row1.z), r13(r13)
, r20(row2.x), r21(row2.y), r22(row2.z), r23(r23)
, r30(row3.x), r31(row3.y), r32(row3.z), r33(r33)
{
}

Matrix4x4::Matrix4x4(
	float r00, float r01, float r02, float r03,
	float r10, float r11, float r12, float r13,
	float r20, float r21, float r22, float r23,
	float r30, float r31, float r32, float r33)
: r00(r00), r01(r01), r02(r02), r03(r03)
, r10(r10), r11(r11), r12(r12), r13(r13)
, r20(r20), r21(r21), r22(r22), r23(r23)
, r30(r30), r31(r31), r32(r32), r33(r33)
{
}

Matrix4x4 Matrix4x4::operator+() const
{
	Matrix4x4 copy = *this;

	for(int i = 0; i < 16; ++i)
	{
		copy[i] = +copy[i];
	}

	return copy;
}

Matrix4x4 Matrix4x4::operator-() const
{
	Matrix4x4 copy = *this;

	for(int i = 0; i < 16; ++i)
	{
		copy[i] = -copy[i];
	}

	return copy;
}

Matrix4x4 &Matrix4x4::operator+=(Matrix4x4 const &other)
{
	*this = *this + other;

	return *this;
}

Matrix4x4 &Matrix4x4::operator-=(Matrix4x4 const &other)
{
	*this = *this - other;

	return *this;
}

Matrix4x4 &Matrix4x4::operator*=(Matrix4x4 const &other)
{
	*this = *this * other;

	return *this;
}

float Matrix4x4::operator[](int index) const
{
	assert(index >= 0 && index < 16);
	return m[index];
}

float &Matrix4x4::operator[](int index)
{
	assert(index >= 0 && index < 16);
	return m[index];
}

static float Det2(
	float r00, float r01,
	float r10, float r11)
{
	return r00 * r11 - r01 * r10;
}

static float Det3(
	float r00, float r01, float r02,
	float r10, float r11, float r12,
	float r20, float r21, float r22)
{
	// [ a b c ]
	// [ d e f ]
	// [ g h i ]

	float const aefhi = r00 * Det2(r11, r12, r21, r22);
	float const bdfgi = r01 * Det2(r10, r12, r20, r22);
	float const cdegh = r02 * Det2(r10, r11, r20, r21);

	return aefhi - bdfgi + cdegh;
}

float Matrix4x4::Determinant() const
{
	// [ a b c d ]
	// [ e f g h ]
	// [ i j k l ]
	// [ m n o p ]

	float const afghjklnop = r00 * Det3(
			r11, r12, r13,
			r21, r22, r23,
			r31, r32, r33);

	float const beghiklmop = r01 * Det3(
			r10, r12, r13,
			r20, r22, r23,
			r30, r32, r33);

	float const cefhijlmnp = r02 * Det3(
			r10, r11, r13,
			r20, r21, r23,
			r30, r31, r33);

	float const defgijkmno = r03 * Det3(
			r10, r11, r12,
			r20, r21, r22,
			r30, r31, r32);

	return afghjklnop - beghiklmop + cefhijlmnp - defgijkmno;
}

Matrix4x4 Matrix4x4::Transpose() const
{
	Matrix4x4 copy = *this;

	for(int i = 0; i < 4; ++i)
	{
		for(int j = 0; j < 4; ++j)
		{
			copy.m2[i][j] = m2[j][i];
		}
	}

	return copy;
}

Matrix4x4 &Matrix4x4::Invert()
{
	float const determinant = Determinant();

	Matrix4x4 cofactor;

	cofactor.m2[0][0] = +Det3(
		m2[1][1], m2[1][2], m2[1][3],
		m2[2][1], m2[2][2], m2[2][3],
		m2[3][1], m2[3][2], m2[3][3]);

	cofactor.m2[0][1] = -Det3(
		m2[1][0], m2[1][2], m2[1][3],
		m2[2][0], m2[2][2], m2[2][3],
		m2[3][0], m2[3][2], m2[3][3]);

	cofactor.m2[0][2] = +Det3(
		m2[1][0], m2[1][1], m2[1][3],
		m2[2][0], m2[2][1], m2[2][3],
		m2[3][0], m2[3][1], m2[3][3]);

	cofactor.m2[0][3] = -Det3(
		m2[1][0], m2[1][1], m2[1][2],
		m2[2][0], m2[2][1], m2[2][2],
		m2[3][0], m2[3][1], m2[3][2]);

	cofactor.m2[1][0] = -Det3(
		m2[0][1], m2[0][2], m2[0][3],
		m2[2][1], m2[2][2], m2[2][3],
		m2[3][1], m2[3][2], m2[3][3]);

	cofactor.m2[1][1] = +Det3(
		m2[0][0], m2[0][2], m2[0][3],
		m2[2][0], m2[2][2], m2[2][3],
		m2[3][0], m2[3][2], m2[3][3]);

	cofactor.m2[1][2] = -Det3(
		m2[0][0], m2[0][1], m2[0][3],
		m2[2][0], m2[2][1], m2[2][3],
		m2[3][0], m2[3][1], m2[3][3]);

	cofactor.m2[1][3] = +Det3(
		m2[0][0], m2[0][1], m2[0][2],
		m2[2][0], m2[2][1], m2[2][2],
		m2[3][0], m2[3][1], m2[3][2]);

	cofactor.m2[2][0] = +Det3(
		m2[0][1], m2[0][2], m2[0][3],
		m2[1][1], m2[1][2], m2[1][3],
		m2[3][1], m2[3][2], m2[3][3]);

	cofactor.m2[2][1] = -Det3(
		m2[0][0], m2[0][2], m2[0][3],
		m2[1][0], m2[1][2], m2[1][3],
		m2[3][0], m2[3][2], m2[3][3]);

	cofactor.m2[2][2] = +Det3(
		m2[0][0], m2[0][1], m2[0][3],
		m2[1][0], m2[1][1], m2[1][3],
		m2[3][0], m2[3][1], m2[3][3]);

	cofactor.m2[2][3] = -Det3(
		m2[0][0], m2[0][1], m2[0][2],
		m2[1][0], m2[1][1], m2[1][2],
		m2[3][0], m2[3][1], m2[3][2]);

	cofactor.m2[3][0] = -Det3(
		m2[0][1], m2[0][2], m2[0][3],
		m2[1][1], m2[1][2], m2[1][3],
		m2[2][1], m2[2][2], m2[2][3]);

	cofactor.m2[3][1] = +Det3(
		m2[0][0], m2[0][2], m2[0][3],
		m2[1][0], m2[1][2], m2[1][3],
		m2[2][0], m2[2][2], m2[2][3]);

	cofactor.m2[3][2] = -Det3(
		m2[0][0], m2[0][1], m2[0][3],
		m2[1][0], m2[1][1], m2[1][3],
		m2[2][0], m2[2][1], m2[2][3]);

	cofactor.m2[3][3] = +Det3(
		m2[0][0], m2[0][1], m2[0][2],
		m2[1][0], m2[1][1], m2[1][2],
		m2[2][0], m2[2][1], m2[2][2]);

	Matrix4x4 const adjugate = cofactor.Transpose();

	*this = (1.f / determinant) * adjugate;

	return *this;
}

Matrix4x4 Matrix4x4::Inverted() const
{
	Matrix4x4 copy = *this;
	return copy.Invert();
}

Matrix4x4 &Matrix4x4::RotateX(float radians)
{
	// [ 1 0    0     0 ]
	// [ 0 cosθ -sinθ 0 ]
	// [ 0 sinθ cosθ  0 ]
	// [ 0 0    0     1 ]

	float const cos = std::cos(radians);
	float const sin = std::sin(radians);

	Matrix4x4 const xRotation(
		1, 0,   0,    0,
		0, cos, -sin, 0,
		0, sin, cos,  0,
		0, 0,   0,    1);

	*this *= xRotation;

	return *this;
}

Matrix4x4 &Matrix4x4::RotateY(float radians)
{
	// [ cosθ  0 sinθ 0 ]
	// [ 0     1 0    0 ]
	// [ -sinθ 0 cosθ 0 ]
	// [ 0     0 0    1 ]

	float const cos = std::cos(radians);
	float const sin = std::sin(radians);

	Matrix4x4 const yRotation(
		cos,  0, sin, 0,
		0,    1, 0,   0,
		-sin, 0, cos, 0,
		0,    0, 0,   1);

	*this *= yRotation;

	return *this;
}

Matrix4x4 &Matrix4x4::RotateZ(float radians)
{
	// [ cosθ -sinθ 0 0 ]
	// [ sinθ cosθ  0 0 ]
	// [ 0    0     1 0 ]
	// [ 0    0     0 1 ]

	float const cos = std::cos(radians);
	float const sin = std::sin(radians);

	Matrix4x4 const zRotation(
		cos, -sin, 0, 0,
		sin, cos,  0, 0,
		0,   0,    1, 0,
		0,   0,    0, 1);

	*this *= zRotation;

	return *this;
}

Matrix4x4 &Matrix4x4::Translate(Vector3 const &translation)
{
	Matrix4x4 const translationMat(
		1, 0, 0, translation.x,
		0, 1, 0, translation.y,
		0, 0, 1, translation.z,
		0, 0, 0, 1);

	*this *= translationMat;

	return *this;
}

Matrix4x4 &Matrix4x4::Scale(Vector3 const &scale)
{
	Matrix4x4 const scaleMatrix(
		scale.x, 0,       0,       0,
		0,       scale.y, 0,       0,
		0,       0,       scale.z, 0,
		0,       0,       0,       1);

	*this *= scaleMatrix;

	return *this;
}

void Matrix4x4::Print() const
{
	using namespace std::string_literals;

	std::string const output
		= "[\n\t"s
		+ "[r00: " + std::to_string(r00)
		+ ", r01: " + std::to_string(r01)
		+ ", r02: " + std::to_string(r02)
		+ ", r03: " + std::to_string(r03)
		+ "],\n\t"
		+ "[r10: " + std::to_string(r10)
		+ ", r11: " + std::to_string(r11)
		+ ", r12: " + std::to_string(r12)
		+ ", r13: " + std::to_string(r13)
		+ "],\n\t"
		+ "[r20: " + std::to_string(r20)
		+ ", r21: " + std::to_string(r21)
		+ ", r22: " + std::to_string(r22)
		+ ", r23: " + std::to_string(r23)
		+ "],\n\t"
		+ "[r30: " + std::to_string(r30)
		+ ", r31: " + std::to_string(r31)
		+ ", r32: " + std::to_string(r32)
		+ ", r33: " + std::to_string(r33)
		+ "],\n""]\n";

	std::puts(output.c_str());

	OutputDebugStringA(output.c_str());
}

Matrix4x4 Matrix4x4::Identity()
{
	Matrix4x4 identity;
	identity.r00 = 1;
	identity.r11 = 1;
	identity.r22 = 1;
	identity.r33 = 1;

	return identity;
}

Matrix4x4 Matrix4x4::LookAt(
	Vector3 const &eye,
	Vector3 const &center,
	Vector3 const &up)
{
	// See https://www.khronos.org/registry/OpenGL-Refpages/gl2.1/xhtml/gluLookAt.xml
	Vector3 const lookDirection = (center - eye).Normalized();
	Vector3 const right = lookDirection.Cross(up).Normalized();
	Vector3 const localUp = right.Cross(lookDirection);

	return Matrix4x4(
		right.x,         localUp.x,         -lookDirection.x,       0,
		right.y,         localUp.y,         -lookDirection.y,       0,
		right.z,         localUp.z,         -lookDirection.z,       0,
		-right.Dot(eye), -localUp.Dot(eye), lookDirection.Dot(eye), 1);
}

Matrix4x4 Matrix4x4::Projection(
	float fovY,
	float aspectRatio,
	float near,
	float far)
{
	// Take tan (t=o/a) of half the fov, to form a right handed triangle
	// with an o-side that's half the desired width
	float const halfHeight = std::tan(fovY * 0.5f);
	float const halfWidth = halfHeight * aspectRatio;

	Vector3 const nearBottomLeft(-halfWidth, -halfHeight, near);
	Vector3 const nearTopRight(halfWidth, halfHeight, near);

	float const rightMinLeft = nearTopRight.x - nearBottomLeft.x;
	float const rightPlusLeft = nearTopRight.x + nearBottomLeft.x;
	float const topMinBottom = nearTopRight.y - nearBottomLeft.y;
	float const topPlusBottom = nearTopRight.y + nearBottomLeft.y;

	float const m00 = 1.f / halfWidth;
	float const m02 = rightPlusLeft / rightMinLeft;
	float const m11 = 1.f / halfHeight;
	float const m12 = topPlusBottom / topMinBottom;
	float const m22 = -(far + near) / (far - near);
	float const m23 = (-2.f * far * near) / (far - near);

	Matrix4x4 const projection(
		m00, 0,   m02, 0,
		0,   m11, m12, 0,
		0,   0,   m22, m23,
		0,   0,    -1, 0);

	return projection;
}

Matrix4x4 operator+(Matrix4x4 const &lhs, Matrix4x4 const &rhs)
{
	return Matrix4x4(
		lhs.r00 + rhs.r00, lhs.r01 + rhs.r01, lhs.r02 + rhs.r02, lhs.r03 + rhs.r03,
		lhs.r10 + rhs.r10, lhs.r11 + rhs.r11, lhs.r12 + rhs.r12, lhs.r13 + rhs.r13,
		lhs.r20 + rhs.r20, lhs.r21 + rhs.r21, lhs.r22 + rhs.r22, lhs.r23 + rhs.r23,
		lhs.r30 + rhs.r30, lhs.r31 + rhs.r31, lhs.r32 + rhs.r32, lhs.r33 + rhs.r33);
}

Matrix4x4 operator-(Matrix4x4 const &lhs, Matrix4x4 const &rhs)
{
	return lhs + -rhs;
}

Matrix4x4 operator*(Matrix4x4 const &lhs, Matrix4x4 const &rhs)
{
	return Matrix4x4(
		// r0
		lhs.r00*rhs.r00 + lhs.r01*rhs.r10 + lhs.r02*rhs.r20 + lhs.r03*rhs.r30,
		lhs.r00*rhs.r01 + lhs.r01*rhs.r11 + lhs.r02*rhs.r21 + lhs.r03*rhs.r31,
		lhs.r00*rhs.r02 + lhs.r01*rhs.r12 + lhs.r02*rhs.r22 + lhs.r03*rhs.r32,
		lhs.r00*rhs.r03 + lhs.r01*rhs.r13 + lhs.r02*rhs.r23 + lhs.r03*rhs.r33,

		// r1
		lhs.r10*rhs.r00 + lhs.r11*rhs.r10 + lhs.r12*rhs.r20 + lhs.r13*rhs.r30,
		lhs.r10*rhs.r01 + lhs.r11*rhs.r11 + lhs.r12*rhs.r21 + lhs.r13*rhs.r31,
		lhs.r10*rhs.r02 + lhs.r11*rhs.r12 + lhs.r12*rhs.r22 + lhs.r13*rhs.r32,
		lhs.r10*rhs.r03 + lhs.r11*rhs.r13 + lhs.r12*rhs.r23 + lhs.r13*rhs.r33,

		// r2
		lhs.r20*rhs.r00 + lhs.r21*rhs.r10 + lhs.r22*rhs.r20 + lhs.r23*rhs.r30,
		lhs.r20*rhs.r01 + lhs.r21*rhs.r11 + lhs.r22*rhs.r21 + lhs.r23*rhs.r31,
		lhs.r20*rhs.r02 + lhs.r21*rhs.r12 + lhs.r22*rhs.r22 + lhs.r23*rhs.r32,
		lhs.r20*rhs.r03 + lhs.r21*rhs.r13 + lhs.r22*rhs.r23 + lhs.r23*rhs.r33,

		// r3
		lhs.r30*rhs.r00 + lhs.r31*rhs.r10 + lhs.r32*rhs.r20 + lhs.r33*rhs.r30,
		lhs.r30*rhs.r01 + lhs.r31*rhs.r11 + lhs.r32*rhs.r21 + lhs.r33*rhs.r31,
		lhs.r30*rhs.r02 + lhs.r31*rhs.r12 + lhs.r32*rhs.r22 + lhs.r33*rhs.r32,
		lhs.r30*rhs.r03 + lhs.r31*rhs.r13 + lhs.r32*rhs.r23 + lhs.r33*rhs.r33);
}

Matrix4x4 operator*(float value, Matrix4x4 const &rhs)
{
	Matrix4x4 copy = rhs;

	for(int i = 0; i < 16; ++i)
	{
		copy[i] = value * rhs[i];
	}

	return copy;
}
} // namespace LibRay::Math
