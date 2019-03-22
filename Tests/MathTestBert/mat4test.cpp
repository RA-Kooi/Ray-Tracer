#include <CppUnitTest.h>
#include <libRay/Math/MathLibrary.hpp>

#include <limits>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace MathTest
{
using flim = std::numeric_limits<float>;

TEST_CLASS(Mat4Test)
{
public:
	void TestEqual(const mat4 &a_Lhs, const mat4 &a_Rhs)
	{
		for (size_t i = 0; i < 16; ++i)
		{
			Assert::AreEqual(a_Lhs.m[i], a_Rhs.m[i]);
		}
	}

	void TestEqual(const mat4 &a_Lhs, const mat4 &a_Rhs, float a_Tolerance)
	{
		for (size_t i = 0; i < 16; ++i)
		{
			const float diff = a_Lhs.m[i] - a_Rhs.m[i];
			Assert::IsTrue((diff <= a_Tolerance && -diff <= a_Tolerance));
		}
	}

	TEST_METHOD(Mat4Construction)
	{
		mat4 matZero = mat4();
		for (size_t i = 0; i < 16; ++i)
		{
			Assert::AreEqual(0.0f, matZero.m[i]);
		}

		const mat4 matVectors(
			vec3(0.6f, 0.2f, 0.3f), 0.4f,
			vec3(0.2f, 0.7f, 0.5f), 0.3f,
			vec3(0.3f, 0.5f, 0.7f), 0.2f,
			vec3(0.4f, 0.3f, 0.2f), 0.6f);

		const mat4 matValues(
			0.6f, 0.2f, 0.3f, 0.4f,
			0.2f, 0.7f, 0.5f, 0.3f,
			0.3f, 0.5f, 0.7f, 0.2f,
			0.4f, 0.3f, 0.2f, 0.6f);

		TestEqual(matValues, matVectors);
	}

	TEST_METHOD(Mat4Identity)
	{
		mat4 matIdent = mat4::Identity();
		matIdent.m[0] = 1.0f;
		matIdent.m[5] = 1.0f;
		matIdent.m[10] = 1.0f;
		matIdent.m[15] = 1.0f;
		for (size_t i = 0; i < 16; i++)
		{
			if (i == 0 || i == 5 || i == 10 || i == 15)
			{
				Assert::AreEqual(1.0f, matIdent.m[i]);
			}
			else
			{
				Assert::AreEqual(0.0f, matIdent.m[i]);
			}
		}
	}

	TEST_METHOD(Mat4Inverse)
	{
		mat4 matOriginal(
			6.0f, 0.0f, -1.0f, 2.0f,
			-1.0f, 2.0f, 3.0f, 6.0f,
			4.0f, -3.0f, 0.0f, 0.0f,
			1.0f, 5.0f, 6.0f, 2.0f);
		mat4 matInverse = matOriginal;
		matInverse.Invert();

		const mat4 matIdentity = mat4::Identity();
		const mat4 calculated = matInverse * matOriginal;

		TestEqual(
			matIdentity,
			calculated,
			3 * flim::epsilon());
	}

	TEST_METHOD(Mat4Determinant)
	{
		mat4 matDeterminant(
			6.0f, 0.0f, -1.0f, 2.0f,
			-1.0f, 2.0f, 3.0f, 6.0f,
			4.0f, -3.0f, 0.0f, 0.0f,
			1.0f, 5.0f, 6.0f, 2.0f);
		Assert::AreEqual(-746.0f, matDeterminant.Determinant());
	}

	TEST_METHOD(Mat4Rotation)
	{
		const mat4 matRotaX(
			1.0f,        0.0f,       0.0f, 0.0f,
			0.0f,  0.2836622f, 0.9589243f, 0.0f,
			0.0f, -0.9589243f, 0.2836622f, 0.0f,
			0.0f,        0.0f,       0.0f, 1.0f );
		TestEqual(
			matRotaX,
			mat4::Identity().RotateX(5.0f),
			3 * flim::epsilon());

		const mat4 matRotaY(
			-0.8011436f, 0.0f,  0.5984721f, 0.0f,
		           0.0f, 1.0f,        0.0f, 0.0f,
		    -0.5984721f, 0.0f, -0.8011436f, 0.0f,
			       0.0f, 0.0f,        0.0f, 1.0f);
		TestEqual(
			matRotaY,
			mat4::Identity().RotateY(2.5f),
			3 * flim::epsilon());

		const mat4 matRotaZ(
			0.5403023f, -0.8414710f, 0.0f, 0.0f,
		    0.8414710f,  0.5403023f, 0.0f, 0.0f,
		          0.0f,        0.0f, 1.0f, 0.0f,
			      0.0f,        0.0f, 0.0f, 1.0f);
		TestEqual(
			matRotaZ,
			mat4::Identity().RotateZ(1.0f),
			3 * flim::epsilon());
	}

	TEST_METHOD(Mat4Translation)
	{
		float x = 65.0f;
		float y = 30.0f;
		float z = 12.0f;
		const mat4 matTranslation(
			1.0f, 0.0f, 0.0f, x,
			0.0f, 1.0f, 0.0f, y,
			0.0f, 0.0f, 1.0f, z,
			0.0f, 0.0f, 0.0f, 1.0f
		);
		mat4 translated = mat4::Identity().Translate(vec3(x, y, z));
		TestEqual(translated, matTranslation);
	}

	TEST_METHOD(Mat4Camera)
	{
		const mat4 matLookAt(
			 1.0f,          0.0f,          0.0f, 0.0f,
			 0.0f,  0.980580688f, -0.196116135f, 0.0f,
			 0.0f,  0.196116135f,  0.980580688f, 0.0f,
			-0.0f, -2.74562597f,   -3.53009057f, 1.0f);
		const mat4 matLookAtCalc = mat4::LookAt(
			vec3(0.0f, 2.0f, 4.0f),
			vec3(0.0f, 3.0f, -1.0f),
			vec3(0.0f, 1.0f, 0.0f));

		TestEqual(
			matLookAt,
			matLookAtCalc,
			3 * flim::epsilon());

		const mat4 matProjection(
		1.29903805f,        0.0f,          0.0f,          0.0f,
		       0.0f, 1.73205090f,          0.0f,          0.0f,
		       0.0f,        0.0f,  -1.00200200f, -0.200200200f,
		       0.0f,        0.0f,         -1.0f,          0.0f);

		const float degToRad = 3.141592653589793238462643383279502884f / 180.0f;
		const mat4 matProjectionCalc = mat4::Projection(
			60.0f * degToRad,
			4.0f / 3.0f,
			0.1f,
			100.0f);

		TestEqual(
			matProjection,
			matProjectionCalc,
			3 * flim::epsilon());
	}

	TEST_METHOD(Mat4Operators)
	{
		const mat4 matOriginal(
			6.0f, 0.0f, -1.0f, 2.0f,
			-1.0f, 2.0f, 3.0f, 6.0f,
			4.0f, -3.0f, 0.0f, 0.0f,
			1.0f, 5.0f, 6.0f, 2.0f);
		mat4 matEqual = matOriginal;
		TestEqual(matEqual, matOriginal);

		const mat4 matOperand(
			-1.0f,  2.0f,  3.0f, 6.0f,
			 1.0f,  5.0f,  6.0f, 2.0f,
			 6.0f,  0.0f, -1.0f, 2.0f,
			 4.0f, -3.0f,  0.0f, 0.0f);

		{
			mat4 matSum = matOriginal;
			matSum += matOperand;
			TestEqual(
				matSum,
				mat4(
					5.0f,  2.0f,  2.0f,  8.0f,
					0.0f,  7.0f,  9.0f,  8.0f,
					10.0f, -3.0f, -1.0f, 2.0f,
					5.0f,  2.0f,  6.0f,  2.0f));

			mat4 matDiff = matOriginal;
			matDiff -= matOperand;
			TestEqual(
				matDiff,
				mat4(
					7.0f,  -2.0f, -4.0f, -4.0f,
					-2.0f, -3.0f, -3.0f, 4.0f,
					-2.0f, -3.0f, 1.0f,  -2.0f,
					-3.0f, 8.0f,  6.0f,  2.0f));

			mat4 matProduct = matOriginal;
			matProduct *= matOperand;
			TestEqual(
				matProduct,
				mat4(
					-4.0f, 6.0f,   19.0f, 34.0f,
					45.0f, -10.0f, 6.0f,  4.0f,
					-7.0f, -7.0f,  -6.0f, 18.0f,
					48.0f, 21.0f,  27.0f, 28.0f));
		}

		{
			const mat4 matSum = matOriginal + matOperand;
			TestEqual(
				matSum,
				mat4(
					5.0f, 2.0f, 2.0f, 8.0f,
					0.0f, 7.0f, 9.0f, 8.0f,
					10.0f, -3.0f, -1.0f, 2.0f,
					5.0f, 2.0f, 6.0f, 2.0f));

			const mat4 matDiff = matOriginal - matOperand;
			TestEqual(
				matDiff,
				mat4(
					7.0f,  -2.0f, -4.0f, -4.0f,
					-2.0f, -3.0f, -3.0f, 4.0f,
					-2.0f, -3.0f, 1.0f,  -2.0f,
					-3.0f, 8.0f,  6.0f,  2.0f));

			const mat4 matProduct = matOriginal * matOperand;
			TestEqual(
				matProduct,
				mat4(
					-4.0f, 6.0f,   19.0f, 34.0f,
					45.0f, -10.0f, 6.0f,  4.0f,
					-7.0f, -7.0f,  -6.0f, 18.0f,
					48.0f, 21.0f,  27.0f, 28.0f));
		}
	}
};
}
