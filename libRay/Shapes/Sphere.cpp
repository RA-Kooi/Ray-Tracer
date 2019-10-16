#include "Sphere.hpp"

#include <cfloat>
#include <cmath>
#include <tuple>
#include <utility>

#include "../Containers/BoundingBox.hpp"
#include "../Math/MathUtils.hpp"
#include "../Math/Matrix.hpp"
#include "../Math/Ray.hpp"
#include "../Math/Vector.hpp"
#include "../Intersection.hpp"

using namespace LibRay::Materials;
using namespace LibRay::Math;

namespace LibRay::Shapes
{
static Vector2 UV(Math::Vector3 const &normal)
{
	float const phi = std::atan2(normal.z, normal.x);
	float const theta = std::asin(-normal.y);

	float const u = 1.f - (phi + Math::PI) / (2.f * Math::PI);
	float const v = (theta + Math::PI / 2.f) / Math::PI;

	return {u, v};
}

std::optional<Intersection> Sphere::Intersects(Ray const &ray) const
{
	// Implicit sphere surface = (point - center)² - radius² = 0
	// Need to find point
	// point is defined as: ray.Origin() + t * ray.Direction()
	// New equation: (ray.Origin() + t * ray.Direction() - center)² - radius² = 0
	// (point - center)² is a perfect square, can rewrite in Ax² + Bx + C form
	// ray.Direction()²t² + 2ray.Direction()(ray.Origin() - center)t
	//     + (ray.origin() - center)² - radius² = 0
	// A = ray.Direction()²
	// B = 2ray.Direction()(ray.Origin() - center)
	// C = (ray.Origin() - center)² - radius²
	// x = t
	// Now we can find x using the quadratic formula
	// x = (-b +- sqrt(b² - 4ac)) / 2a

	Matrix4x4 const worldToModel = transform.InverseMatrix();

	Ray const modelRay(
		Transform::TransformTranslation(worldToModel, ray.Origin()),
		Transform::TransformDirection(worldToModel, ray.Direction()));

	Vector3 const centerToOrigin = modelRay.Origin();

	float const a = glm::length2(modelRay.Direction());
	float const b = glm::dot((modelRay.Direction() * 2.f), centerToOrigin);
	float const c = glm::length2(centerToOrigin) - 1.f;

	std::tuple<int, float, float> solutions = Math::SolveQuadratic(a, b, c);

	int const solutionCount = std::get<0>(solutions);
	float solution1 = std::get<1>(solutions);
	float solution2 = std::get<2>(solutions);

	if(solutionCount == 0)
		return std::nullopt;

	if(solutionCount == 1)
	{
		Vector3 const positionOnSphere = modelRay.Origin()
			+ modelRay.Direction()
			* solution1;

		return Intersection(
			*this,
			positionOnSphere,
			Transform::TransformTranslation(transform.Matrix(), positionOnSphere),
			UV(glm::normalize(positionOnSphere)));
	}

	// Arrange the points to along the line so that solution1 is always in
	// the direction towards the screen first.
	if(solution1 > solution2)
		std::swap(solution1, solution2);

	// The first point is behind the ray origin.
	if(solution1 < 0)
	{
		solution1 = solution2;

		// Both points are behind the ray origin.
		if(solution1 < 0)
			return std::nullopt;
	}

	Vector3 const positionOnSphere = modelRay.Origin()
		+ modelRay.Direction()
		* solution1;

	Matrix4x4 const &matrix = transform.Matrix();

	Vector3 const normal =
		Transform::TransformDirection(matrix, positionOnSphere);

	return Intersection(
		*this,
		normal,
		Transform::TransformTranslation(matrix, positionOnSphere),
		UV(positionOnSphere));
}

Containers::BoundingBox Sphere::CalculateBoundingBox() const
{
	return Containers::BoundingBox(transform.Scale(), transform.Position());
}
} // namespace LibRay::Shapes
