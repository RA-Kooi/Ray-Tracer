#include <CppUnitTest.h>
#include <libRay/Math/MathLibrary.hpp>

#include <limits>
#include <random>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace MathTest
{
using flim = std::numeric_limits<float>;

TEST_CLASS(Vec2Tests)
{
public:
	void TestEqual(const vec2 &a_Lhs, const vec2 &a_Rhs)
	{
		for (size_t i = 0; i < 2; ++i)
		{
			Assert::AreEqual(a_Lhs.v[i], a_Rhs.v[i]);
		}
	}

	TEST_METHOD(Vec2Construction)
	{
		const vec2 vecEmpty;
		Assert::AreEqual(vecEmpty.v[0], 0.0f);
		Assert::AreEqual(vecEmpty.v[1], 0.0f);

		const vec2 vecValues(1.0f, 4.0f);
		Assert::AreEqual(vecValues.v[0], 1.0f);
		Assert::AreEqual(vecValues.v[1], 4.0f);
	}

	TEST_METHOD(Vec2Assignment)
	{
		const vec2 vecOriginal(5.0f, 7.0f);
		vec2 vecEqual = vecOriginal;
		TestEqual(vecEqual, vecOriginal);
	}

	TEST_METHOD(Vec2Operators)
	{
		const vec2 vecOriginal(5.0f, 7.0f);
		const vec2 vecOperand(6.0f, 3.0f);
		{
			vec2 vecSum = vecOriginal;
			vecSum += vecOperand;
			TestEqual(vecSum, vec2(11.0f, 10.0f));

			vec2 vecDiff = vecOriginal;
			vecDiff -= vecOperand;
			TestEqual(vecDiff, vec2(-1.0f, 4.0f));
		}

		{
			const vec2 vecSum = vecOriginal + vecOperand;
			TestEqual(vecSum, vec2(11.0f, 10.0f));

			const vec2 vecDiff = vecOriginal - vecOperand;
			TestEqual(vecDiff, vec2(-1.0f, 4.0f));
		}

		{
			vec2 vecDiv = vec2(27.0f, 6.0f);
			vecDiv /= 3.0f;
			TestEqual(vecDiv, vec2(9.0f, 2.0f));

			vec2 vecMult = vecDiv;
			vecMult *= 2.5f;
			TestEqual(vecMult, vec2(22.5f, 5.0f));
		}

		{
			vec2 vecDiv = vec2(27.0f, 6.0f) / 3.0f;
			TestEqual(vecDiv, vec2(9.0f, 2.0f));

			vec2 vecMult = vecDiv * 2.5f;
			TestEqual(vecMult, vec2(22.5f, 5.0f));
		}
	}

	TEST_METHOD(Vec2Equality)
	{
		std::random_device dev;
		std::mt19937 generator(dev());
		std::uniform_real_distribution<float> dist(flim::min(), flim::max());
		const float x = dist(generator);
		const float y = dist(generator);
		const vec2 lhs(x, y);

		Assert::IsTrue(lhs == vec2(x, y));
		Assert::IsFalse(lhs == vec2(y, x));

		Assert::IsFalse(lhs != vec2(x, y));
		Assert::IsTrue(lhs != vec2(y, x));
	}

	TEST_METHOD(Vec2DotProduct)
	{
		const vec2 lhs(2.0f, 3.0f);
		const vec2 rhs(5.0f, 4.0f);

		const float dotProduct = 22.0f;
		Assert::AreEqual(lhs.Dot(rhs), dotProduct);
	}

	TEST_METHOD(Vec2Normalize)
	{
		vec2 vector(3.0f, 4.0f);
		vector.Normalize();
		Assert::AreEqual(vector.Magnitude(), 1.0f);
	}

	TEST_METHOD(Vec2Length)
	{
		const vec2 vector(3.0f, 4.0f);
		Assert::AreEqual(vector.Magnitude(), 5.0f);
	}
};
}
