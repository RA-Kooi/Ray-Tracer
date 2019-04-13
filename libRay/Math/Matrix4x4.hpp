#ifndef f19f0a96_596e_51fa_cfeb_d8d421a1c271
#define f19f0a96_596e_51fa_cfeb_d8d421a1c271

#include "../API.hpp"
#include "Vector3.hpp"

namespace LibRay::Math
{
class LIBRAY_API Matrix4x4
{
public:
	union
	{
		struct
		{
			float r00; float r01; float r02; float r03;
			float r10; float r11; float r12; float r13;
			float r20; float r21; float r22; float r23;
			float r30; float r31; float r32; float r33;
		};

		float m[16];
		float m2[4][4];
	};

public:
	Matrix4x4();
	Matrix4x4(
		Vector3 row0, float r03,
		Vector3 row1, float r13,
		Vector3 row2, float r23,
		Vector3 row3, float r33);
	Matrix4x4(
		float r00, float r01, float r02, float r03,
		float r10, float r11, float r12, float r13,
		float r20, float r21, float r22, float r23,
		float r30, float r31, float r32, float r33);

	Matrix4x4(Matrix4x4 const &other) = default;

	Matrix4x4& operator=(Matrix4x4 const &other) = default;

	Matrix4x4 operator+() const;
	Matrix4x4 operator-() const;

	Matrix4x4& operator+=(Matrix4x4 const &other);
	Matrix4x4& operator-=(Matrix4x4 const &other);
	Matrix4x4& operator*=(Matrix4x4 const &other);
	float operator[](int index) const;
	float &operator[](int index);

	float Determinant() const;
	Matrix4x4 Transpose() const;

	Matrix4x4& Invert();
	Matrix4x4 Inverted() const;

	Matrix4x4 &RotateX(float radians);
	Matrix4x4 &RotateY(float radians);
	Matrix4x4 &RotateZ(float radians);
	Matrix4x4 &Translate(Vector3 const &translation);
	Matrix4x4 &Scale(Vector3 const &scale);

	void Print() const;

	static Matrix4x4 Identity();

	static Matrix4x4 LookAt(
		Vector3 const &eye,
		Vector3 const &center,
		Vector3 const &up);

	static Matrix4x4 Projection(
		float fovY,
		float aspectRatio,
		float near,
		float far);
};

LIBRAY_API Matrix4x4 operator+(Matrix4x4 const &lhs, Matrix4x4 const &rhs);
LIBRAY_API Matrix4x4 operator-(Matrix4x4 const &lhs, Matrix4x4 const &rhs);
LIBRAY_API Matrix4x4 operator*(Matrix4x4 const &lhs, Matrix4x4 const &rhs);
LIBRAY_API Matrix4x4 operator*(float value, Matrix4x4 const &rhs);
} // namespace LibRay::Math

using mat4 = LibRay::Math::Matrix4x4;

#endif // f19f0a96_596e_51fa_cfeb_d8d421a1c271
