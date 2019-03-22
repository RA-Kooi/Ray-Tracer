#include <CppUnitTest.h>

#include <random>

#include <libRay/Math/Vector3.hpp>
#include <libRay/Utilites.hpp>

#include <glm/gtx/norm.hpp>
#include <glm/vec3.hpp>
#include <glm/geometric.hpp>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace LibRay::Math;

namespace MathTestGLM
{
TEST_CLASS(Vector3Test)
{
public:
	Vector3Test()
	: engine(std::random_device{}())
	, dist(-10.f, 10.f)
	{
	}

	Vector3 Random()
	{
		return Vector3(dist(engine), dist(engine), dist(engine));
	}

	void Equal(Vector3 const &a, glm::vec3 const &b)
	{
		Assert::IsTrue(AlmostEqual(a.x, b.x));
		Assert::IsTrue(AlmostEqual(a.y, b.y));
		Assert::IsTrue(AlmostEqual(a.z, b.z));
	}

	TEST_METHOD(TestOperators)
	{
		{
			Vector3 a = Random();
			Vector3 const b = Random();
			glm::vec3 glmA(a.x, a.y, a.z);
			glm::vec3 const glmB(b.x, b.y, b.z);

			a += b;
			glmA += glmB;

			Equal(a, glmA);
		}

		{
			Vector3 a = Random();
			Vector3 const b = Random();
			glm::vec3 glmA(a.x, a.y, a.z);
			glm::vec3 const glmB(b.x, b.y, b.z);

			a -= b;
			glmA -= glmB;

			Equal(a, glmA);
		}

		{
			Vector3 a = Random();
			glm::vec3 glmA(a.x, a.y, a.z);

			float const value = dist(engine);

			a *= value;
			glmA *= value;

			Equal(a, glmA);
		}

		{
			Vector3 a = Random();
			glm::vec3 glmA(a.x, a.y, a.z);

			float const value = dist(engine);

			a /= value;
			glmA /= value;

			Equal(a, glmA);
		}

		{
			Vector3 const a = Random();

			Assert::AreEqual(a.x, a[0]);
			Assert::AreEqual(a.y, a[1]);
			Assert::AreEqual(a.z, a[2]);

			Vector3 b = Random();

			Assert::AreEqual(b.x, b[0]);
			Assert::AreEqual(b.y, b[1]);
			Assert::AreEqual(b.z, b[2]);
		}

		{
			Vector3 const a = Random();
			Vector3 const b = a;

			Assert::IsTrue(a == b);
		}

		{
			Vector3 const a = Random(), b = Random();

			Assert::IsTrue(a != b);
		}
	}

	TEST_METHOD(TestDot)
	{
		Vector3 const a = Random(), b = Random();
		glm::vec3 const glmA(a.x, a.y, a.z), glmB(b.x, b.y, b.z);

		float const myDot = a.Dot(b);
		float const glmDot = glm::dot(glmA, glmB);

		Assert::IsTrue(AlmostEqual(myDot, glmDot));
	}

	TEST_METHOD(TestMagnitude)
	{
		Vector3 const a = Random();
		glm::vec3 const glmA(a.x, a.y, a.z);

		float const mySquareMagnitude = a.SquareMagnitude();
		float const myMagnitude = a.Magnitude();

		float const glmSquareMagnitude = glm::length2(glmA);
		float const glmMagnitude = glm::length(glmA);

		Assert::IsTrue(AlmostEqual(mySquareMagnitude, glmSquareMagnitude));
		Assert::IsTrue(AlmostEqual(myMagnitude, glmMagnitude));
	}

	TEST_METHOD(TestCross)
	{
		Vector3 const a = Random(), b = Random();
		glm::vec3 const glmA(a.x, a.y, a.z), glmB(b.x, b.y, b.z);

		Vector3 const myCross = a.Cross(b);
		glm::vec3 const glmCross = glm::cross(glmA, glmB);

		Equal(myCross, glmCross);
	}

private:
	std::mt19937 engine;
	std::uniform_real_distribution<float> dist;
};
}
