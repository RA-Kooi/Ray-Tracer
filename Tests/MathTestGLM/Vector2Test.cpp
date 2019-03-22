#include <CppUnitTest.h>

#include <random>

#include <libRay/Math/Vector2.hpp>
#include <libRay/Utilites.hpp>

#include <glm/gtx/norm.hpp>
#include <glm/vec2.hpp>
#include <glm/geometric.hpp>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace LibRay::Math;

namespace MathTestGLM
{
TEST_CLASS(Vector2Test)
{
public:
	Vector2Test()
	: engine(std::random_device{}())
	, dist(-10.f, 10.f)
	{
	}

	Vector2 Random()
	{
		return Vector2(dist(engine), dist(engine));
	}

	void Equal(Vector2 const &a, glm::vec2 const &b)
	{
		Assert::IsTrue(AlmostEqual(a.x, b.x));
		Assert::IsTrue(AlmostEqual(a.y, b.y));
	}

	TEST_METHOD(TestOperators)
	{
		{
			Vector2 a = Random();
			Vector2 const b = Random();
			glm::vec2 glmA(a.x, a.y);
			glm::vec2 const glmB(b.x, b.y);

			a += b;
			glmA += glmB;

			Equal(a, glmA);
		}

		{
			Vector2 a = Random();
			Vector2 const b = Random();
			glm::vec2 glmA(a.x, a.y);
			glm::vec2 const glmB(b.x, b.y);

			a -= b;
			glmA -= glmB;

			Equal(a, glmA);
		}

		{
			Vector2 a = Random();
			glm::vec2 glmA(a.x, a.y);

			float const value = dist(engine);

			a *= value;
			glmA *= value;

			Equal(a, glmA);
		}

		{
			Vector2 a = Random();
			glm::vec2 glmA(a.x, a.y);

			float const value = dist(engine);

			a /= value;
			glmA /= value;

			Equal(a, glmA);
		}

		{
			Vector2 const a = Random();

			Assert::AreEqual(a.x, a[0]);
			Assert::AreEqual(a.y, a[1]);

			Vector2 b = Random();

			Assert::AreEqual(b.x, b[0]);
			Assert::AreEqual(b.y, b[1]);
		}

		{
			Vector2 const a = Random();
			Vector2 const b = a;

			Assert::IsTrue(a == b);
		}

		{
			Vector2 const a = Random(), b = Random();

			Assert::IsTrue(a != b);
		}
	}

	TEST_METHOD(TestDot)
	{
		Vector2 const a = Random(), b = Random();
		glm::vec2 const glmA(a.x, a.y), glmB(b.x, b.y);

		float const myDot = a.Dot(b);
		float const glmDot = glm::dot(glmA, glmB);

		Assert::IsTrue(AlmostEqual(myDot, glmDot));
	}

	TEST_METHOD(TestMagnitude)
	{
		Vector2 const a = Random();
		glm::vec2 const glmA(a.x, a.y);

		float const mySquareMagnitude = a.SquareMagnitude();
		float const myMagnitude = a.Magnitude();

		float const glmSquareMagnitude = glm::length2(glmA);
		float const glmMagnitude = glm::length(glmA);

		Assert::IsTrue(AlmostEqual(mySquareMagnitude, glmSquareMagnitude));
		Assert::IsTrue(AlmostEqual(myMagnitude, glmMagnitude));
	}

private:
	std::mt19937 engine;
	std::uniform_real_distribution<float> dist;
};
}
