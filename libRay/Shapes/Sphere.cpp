#include "Sphere.hpp"

#include <float.h>
#include <tuple>
#include <utility>

#include "../Containers/BoundingBox.hpp"
#include "../Math/MathUtils.hpp"
#include "../Math/Ray.hpp"
#include "../Intersection.hpp"

using namespace LibRay::Math;

namespace LibRay::Shapes
{
Sphere::Sphere(
	float radius,
	class Transform const &transform,
	MaterialStore const &materialStore,
	MaterialStore::IndexType materialIndex,
	class Color const &color)
: Shape(transform, materialStore, materialIndex, color)
, radius(radius)
{
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

	Matrix4x4 const worlToModel = transform.InverseMatrix();

	Ray const modelRay(
		Transform::TransformTranslation(worlToModel, ray.Origin()),
		Transform::TransformDirection(worlToModel, ray.Direction()));

	Vector3 const centerToOrigin = modelRay.Origin();

	float const a = modelRay.Direction().squaredNorm();
	float const b = (modelRay.Direction() * 2.f).dot(centerToOrigin);
	float const c = centerToOrigin.squaredNorm() - radius * radius;

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
			Transform::TransformTranslation(transform.Matrix(), positionOnSphere));
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

	return Intersection(
		*this,
		Transform::TransformDirection(transform.Matrix(), positionOnSphere),
		Transform::TransformTranslation(transform.Matrix(), positionOnSphere));
}

Containers::BoundingBox Sphere::CalculateBoundingBox() const
{
	Vector3 const radiusVec(radius, radius, radius);

	Vector3 bounds = (radiusVec * 2).cwiseProduct(transform.Scale());
	if(bounds.x() < FLT_EPSILON)
		bounds.x() = FLT_EPSILON;
	if(bounds.y() < FLT_EPSILON)
		bounds.y() = FLT_EPSILON;
	if(bounds.z() < FLT_EPSILON)
		bounds.z() = FLT_EPSILON;

	return Containers::BoundingBox(bounds, transform.Position());
}
} // namespace LibRay::Shapes
