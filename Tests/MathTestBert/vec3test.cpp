#include <CppUnitTest.h>
#include <libRay/Math/MathLibrary.hpp>

#include <limits>
#include <random>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace MathTest
{
using flim = std::numeric_limits<float>;

TEST_CLASS(Vec3Test)
{
public:
	void TestEqual(float a_Lhs, float a_Rhs, float a_Tolerance)
	{
		const float diff = a_Lhs - a_Rhs;
		Assert::IsTrue((diff <= a_Tolerance && -diff <= a_Tolerance));
	}

	void TestEqual(const vec3 &a_Lhs, const vec3 &a_Rhs)
	{
		for (size_t i = 0; i < 3; ++i)
		{
			Assert::AreEqual(a_Lhs.v[i], a_Rhs.v[i]);
		}
	}

	TEST_METHOD(Vec3Construction)
	{
		const vec3 vecEmpty;
		Assert::AreEqual(vecEmpty.v[0], 0.0f);
		Assert::AreEqual(vecEmpty.v[1], 0.0f);
		Assert::AreEqual(vecEmpty.v[2], 0.0f);

		const vec3 vecValues(1.0f, 4.0f, 5.0f);
		Assert::AreEqual(vecValues.v[0], 1.0f);
		Assert::AreEqual(vecValues.v[1], 4.0f);
		Assert::AreEqual(vecValues.v[2], 5.0f);
	}

	TEST_METHOD(Vec3Assignment)
	{
		const vec3 vecOriginal(5.0f, 7.0f, 2.0f);
		vec3 vecEqual = vecOriginal;
		TestEqual(vecEqual, vecOriginal);
	}

	TEST_METHOD(Vec3Operators)
	{
		const vec3 vecOriginal(5.0f, 7.0f, 2.0f);
		const vec3 vecOperand(6.0f, 3.0f, 1.0f);
		{
			vec3 vecSum = vecOriginal;
			vecSum += vecOperand;
			TestEqual(vecSum, vec3(11.0f, 10.0f, 3.0f));

			vec3 vecDiff = vecOriginal;
			vecDiff -= vecOperand;
			TestEqual(vecDiff, vec3(-1.0f, 4.0f, 1.0f));
		}

		{
			const vec3 vecSum = vecOriginal + vecOperand;
			TestEqual(vecSum, vec3(11.0f, 10.0f, 3.0f));

			const vec3 vecDiff = vecOriginal - vecOperand;
			TestEqual(vecDiff, vec3(-1.0f, 4.0f, 1.0f));
		}

		{
			vec3 vecDiv = vec3(6.0f, 27.0f, 3.0f);
			vecDiv /= 3.0f;
			TestEqual(vecDiv, vec3(2.0f, 9.0f, 1.0f));

			vec3 vecMult = vecDiv;
			vecMult *= 2.5f;
			TestEqual(vecMult, vec3(5.0f, 22.5f, 2.5f));
		}

		{
			vec3 vecDiv = vec3(6.0f, 27.0f, 3.0f) / 3.0f;
			TestEqual(vecDiv, vec3(2.0f, 9.0f, 1.0f));

			vec3 vecMult = vecDiv * 2.5f;
			TestEqual(vecMult, vec3(5.0f, 22.5f, 2.5f));
		}
	}

	TEST_METHOD(Vec3Equality)
	{
		std::random_device dev;
		std::mt19937 generator(dev());
		std::uniform_real_distribution<float> dist(flim::min(), flim::max());
		const float x = dist(generator);
		const float y = dist(generator);
		const float z = dist(generator);
		const vec3 lhs(x, y, z);

		Assert::IsTrue(lhs == vec3(x, y, z));
		Assert::IsFalse(lhs == vec3(y, x, z));

		Assert::IsFalse(lhs != vec3(x, y, z));
		Assert::IsTrue(lhs != vec3(y, x, z));
	}

	TEST_METHOD(Vec3DotProduct)
	{
		const vec3 lhs(2.0f, 3.0f, 6.0f);
		const vec3 rhs(5.0f, 4.0f, 1.0f);

		const float dotProduct = 28.0f;
		Assert::AreEqual(lhs.Dot(rhs), dotProduct);
	}

	TEST_METHOD(Vec3CrossProduct)
	{
		vec3 lhs(1.0f, 2.0f, 3.0f);
		vec3 rhs(4.0f, 5.0f, 6.0f);
		Assert::IsTrue(vec3(-3.0f, 6.0f, -3.0f) == lhs.Cross(rhs));
	}

	TEST_METHOD(Vec3Normalize)
	{
		vec3 vector(3.0f, 4.0f, 5.0f);
		vector.Normalize();
		Assert::AreEqual(vector.Magnitude(), 1.0f);
	}

	TEST_METHOD(Vec3Length)
	{
		const vec3 vector(3.0f, 4.0f, 5.0f);
		TestEqual(
			vector.Magnitude(),
			sqrtf(50.0f),
			3 * flim::epsilon());
	}
};
}
