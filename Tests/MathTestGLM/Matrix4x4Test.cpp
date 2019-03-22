#include <CppUnitTest.h>

#include <random>

#include <Windows.h>
#undef near
#undef far

#include <libRay/Math/Matrix4x4.hpp>
#include <libRay/Utilites.hpp>

#include <glm/gtx/matrix_major_storage.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/geometric.hpp>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace LibRay::Math;

namespace MathTestGLM
{
TEST_CLASS(Matrix4x4Test)
{
public:
	Matrix4x4Test()
		: engine(std::random_device{}())
		, dist(-10.f, 10.f)
	{
	}

	Matrix4x4 Random()
	{
		return Matrix4x4(
			dist(engine), dist(engine), dist(engine), dist(engine),
			dist(engine), dist(engine), dist(engine), dist(engine),
			dist(engine), dist(engine), dist(engine), dist(engine),
			dist(engine), dist(engine), dist(engine), dist(engine));
	}

	glm::mat4 ToGLM(Matrix4x4 const &a)
	{
		glm::mat4 temp;

		for(int i = 0; i < 4; ++i)
		{
			for(int j = 0; j < 4; ++j)
			{
				temp[j][i] = a.m2[i][j];
			}
		}

		return temp;
	}

	void Equal(
		Matrix4x4 const &a,
		glm::mat4 const &b,
		float epsilon = FLT_EPSILON)
	{
		for(int i = 0; i < 4; ++i)
		{
			for(int j = 0; j < 4; ++j)
			{
				Assert::IsTrue(AlmostEqual(a.m2[i][j], b[j][i], epsilon));
			}
		}
	}

	TEST_METHOD(TestIdentity)
	{
		Equal(Matrix4x4::Identity(), glm::identity<glm::mat4>());
	}

	TEST_METHOD(TestOperators)
	{
		{
			Matrix4x4 a = Random();
			glm::mat4 glmA = ToGLM(a);

			Matrix4x4 const b = Random();
			glm::mat4 const glmB = ToGLM(b);

			Matrix4x4 const result = a + b;
			glm::mat4 const glmResult = glmA + glmB;

			Equal(result, glmResult);
		}

		{
			Matrix4x4 a = Random();
			glm::mat4 glmA = ToGLM(a);

			Matrix4x4 const b = Random();
			glm::mat4 const glmB = ToGLM(b);

			Matrix4x4 const result = a - b;
			glm::mat4 const glmResult = glmA - glmB;

			Equal(result, glmResult);
		}

		{
			Matrix4x4 a = Random();
			glm::mat4 glmA = ToGLM(a);

			Matrix4x4 const b = Random();
			glm::mat4 const glmB = ToGLM(b);

			Matrix4x4 const result = a * b;
			glm::mat4 const glmResult = glmA * glmB;

			Equal(result, glmResult);
		}
	}

	TEST_METHOD(TestDeterminant)
	{
		Matrix4x4 const a = Random();
		glm::mat4 const glmA = ToGLM(a);

		float const myDeterminant = a.Determinant();
		float const glmDeterminant = glm::determinant(glmA);

		Assert::IsTrue(AlmostEqual(myDeterminant, glmDeterminant, 0.002f));
	}

	TEST_METHOD(TestTranspose)
	{
		Matrix4x4 const a = Random();
		glm::mat4 const glmA = ToGLM(a);

		Matrix4x4 const transposed = a.Transpose();
		glm::mat4 const glmTransposed = glm::transpose(glmA);

		Equal(transposed, glmTransposed);
	}

	TEST_METHOD(TestInverse)
	{
		Matrix4x4 a = Random();
		while(a.Determinant() == 0.f)
			a = Random();

		glm::mat4 const glmA = ToGLM(a);

		Matrix4x4 const inverse = a.Inverted();
		glm::mat4 const glmInverse = glm::inverse(glmA);

		Matrix4x4 const result = a * inverse;
		glm::mat4 const glmResult = glmA * glmInverse;

		Equal(result, glmResult, 15.f * FLT_EPSILON);
		Equal(a, glmA, 15.f * FLT_EPSILON);
	}

	TEST_METHOD(TestRotation)
	{
		std::uniform_real_distribution<float> radDist(
			-glm::pi<float>(),
			glm::pi<float>());

		float const xRotation = radDist(engine);
		float const yRotation = radDist(engine);
		float const zRotation = radDist(engine);

		Matrix4x4 rotatedX = Random();
		Matrix4x4 rotatedY = Random();
		Matrix4x4 rotatedZ = Random();

		glm::mat4 glmRotatedX = ToGLM(rotatedX);
		glm::mat4 glmRotatedY = ToGLM(rotatedY);
		glm::mat4 glmRotatedZ = ToGLM(rotatedZ);

		rotatedX.RotateX(xRotation);
		rotatedY.RotateY(yRotation);
		rotatedZ.RotateZ(zRotation);

		glmRotatedX = glm::rotate(glmRotatedX, xRotation, glm::vec3(1, 0, 0));
		glmRotatedY = glm::rotate(glmRotatedY, yRotation, glm::vec3(0, 1, 0));
		glmRotatedZ = glm::rotate(glmRotatedZ, zRotation, glm::vec3(0, 0, 1));

		Equal(rotatedX, glmRotatedX, 8.f * FLT_EPSILON);
		Equal(rotatedY, glmRotatedY, 8.f * FLT_EPSILON);
		Equal(rotatedZ, glmRotatedZ, 8.f * FLT_EPSILON);
	}

	TEST_METHOD(TestTranslation)
	{
		Matrix4x4 translated = Random();
		glm::mat4 const untranslated = ToGLM(translated);

		Vector3 const translation(dist(engine), dist(engine), dist(engine));
		glm::vec3 const glmTranslation(
			translation.x,
			translation.y,
			translation.z);

		translated.Translate(translation);
		glm::mat4 const glmTranslated = glm::translate(
			untranslated,
			glmTranslation);

		Equal(translated, glmTranslated);
	}

	TEST_METHOD(TestProjection)
	{
		float const fovY = glm::radians(+dist(engine));
		float const aspectRatio = +dist(engine)/+dist(engine);
		float const near = +dist(engine);
		float const far = near + +dist(engine);

		Matrix4x4 const projection =
			Matrix4x4::Projection(fovY, aspectRatio, near, far);

		glm::mat4 const glmProjection =
			glm::perspective(fovY, aspectRatio, near, far);

		Equal(projection, glmProjection);
	}

	TEST_METHOD(TestLookAt)
	{
		Vector3 const eye(dist(engine), dist(engine), dist(engine));
		glm::vec3 const glmEye(eye.x, eye.y, eye.z);

		Vector3 const center = eye
			+ Vector3(dist(engine), dist(engine), dist(engine));
		glm::vec3 const glmCenter(center.x, center.y, center.z);

		Vector3 const up(0, dist(engine), 0);
		glm::vec3 const glmUp(up.x, up.y, up.z);

		Matrix4x4 const lookAt = Matrix4x4::LookAt(eye, center, up);
		glm::mat4 const glmLookAt = glm::lookAt(glmEye, glmCenter, glmUp);

		// We are in column major order
		for(int i = 0; i < 4; ++i)
		{
			for(int j = 0; j < 4; ++j)
			{
				Assert::IsTrue(
					AlmostEqual(
						lookAt[(i * 4) + j],
						glmLookAt[i][j],
						0.000001f));
			}
		}
	}

private:
	std::mt19937 engine;
	std::uniform_real_distribution<float> dist;
};
}
