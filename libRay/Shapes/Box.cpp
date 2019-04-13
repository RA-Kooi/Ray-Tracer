#include "Box.hpp"

#include <algorithm>

#include "../Containers/BoundingBox.hpp"
#include "../Math/MathUtils.hpp"
#include "../Math/Ray.hpp"
#include "../Math/Vector2.hpp"
#include "../Intersection.hpp"
#include "../Transform.hpp"

namespace LibRay
{
using namespace Math;
using namespace Shapes;

std::optional<Intersection> Box::Intersects(Math::Ray const &ray) const
{
	Matrix4x4 const worlToModel = transform.InverseMatrix();
	Ray const modelRay(
		Transform::TransformTranslation(worlToModel, ray.Origin()),
		Transform::TransformDirection(worlToModel, ray.Direction()));

	Vector3 const halfBoundaries(0.5f, 0.5f, 0.5f);

	Vector3 const inverseDir = 1.f / modelRay.Direction();
	Vector3 const hit1 = (-halfBoundaries - modelRay.Origin()) * inverseDir;
	Vector3 const hit2 = (halfBoundaries - modelRay.Origin()) * inverseDir;

	Vector3 const min = hit1.Min(hit2);
	Vector3 const max = hit1.Max(hit2);

	float const near = min.MaxComponent();
	float const far = max.MinComponent();

	if(far < near)
		return std::nullopt;

	float distanceToIntersection = near;

	if(distanceToIntersection < 0)
	{
		distanceToIntersection = far;

		if(distanceToIntersection < 0)
			return std::nullopt;
	}

	Vector3 const pointOnBox = modelRay.Origin()
		+ modelRay.Direction()
		* distanceToIntersection;

	auto const calculateNormal = [](Vector3 const &point) -> Vector3
	{
		Vector3 const pointAbs = point.Abs();
		Vector3 normal;

		if(pointAbs.x >= pointAbs.y && pointAbs.x >= pointAbs.z)
			normal.x = float(Sign(point.x));

		if(pointAbs.y >= pointAbs.x && pointAbs.y >= pointAbs.z)
			normal.y = float(Sign(point.y));

		if(pointAbs.z >= pointAbs.x && pointAbs.z >= pointAbs.y)
			normal.z = float(Sign(point.z));

		if(normal.x > 0 && normal.y > 0)
			normal.y = 0;
		if(normal.x > 0 && normal.z > 0)
			normal.z = 0;
		if(normal.y > 0 && normal.z > 0)
			normal.z = 0;

		return normal;
	};

	Vector3 const normal = calculateNormal(pointOnBox);

	return Intersection(
		*this,
		Transform::TransformDirection(transform.Matrix(), normal),
		Transform::TransformTranslation(transform.Matrix(), pointOnBox));
}

Containers::BoundingBox Box::CalculateBoundingBox() const
{
	Vector3 const frontBottomLeft(-0.5, -0.5, -0.5);
	Vector3 const frontBottomRight(0.5, -0.5, -0.5);
	Vector3 const frontTopLeft(-0.5, 0.5, -0.5);
	Vector3 const frontTopRight(0.5, 0.5, -0.5);

	Vector3 const backBottomLeft(-0.5, -0.5, 0.5);
	Vector3 const backBottomRight(0.5, -0.5, 0.5);
	Vector3 const backTopLeft(-0.5, 0.5, 0.5);
	Vector3 const backTopRight(0.5, 0.5, 0.5);

	Vector3 const worldFrontBottomLeft =
		Transform::TransformDirection(transform.Matrix(), frontBottomLeft);
	Vector3 const worldFrontBottomRight =
		Transform::TransformDirection(transform.Matrix(), frontBottomRight);
	Vector3 const worldFrontTopLeft =
		Transform::TransformDirection(transform.Matrix(), frontTopLeft);
	Vector3 const worldFrontTopRight =
		Transform::TransformDirection(transform.Matrix(), frontTopRight);

	Vector3 const worldBackBottomLeft =
		Transform::TransformDirection(transform.Matrix(), backBottomLeft);
	Vector3 const worldBackBottomRight =
		Transform::TransformDirection(transform.Matrix(), backBottomRight);
	Vector3 const worldBackTopLeft =
		Transform::TransformDirection(transform.Matrix(), backTopLeft);
	Vector3 const worldBackTopRight =
		Transform::TransformDirection(transform.Matrix(), backTopRight);

	Vector3 const min = worldFrontBottomLeft
		.Min(worldFrontBottomRight)
		.Min(worldFrontTopLeft)
		.Min(worldFrontTopRight)
		.Min(worldBackBottomLeft)
		.Min(worldBackBottomRight)
		.Min(worldBackTopLeft)
		.Min(worldBackTopRight);

	Vector3 const max = worldFrontBottomLeft
		.Max(worldFrontBottomRight)
		.Max(worldFrontTopLeft)
		.Max(worldFrontTopRight)
		.Max(worldBackBottomLeft)
		.Max(worldBackBottomRight)
		.Max(worldBackTopLeft)
		.Max(worldBackTopRight);

	Vector3 bounds = (max - min).Abs();
	if(bounds.x < FLT_EPSILON)
		bounds.x = FLT_EPSILON;
	if(bounds.y < FLT_EPSILON)
		bounds.y = FLT_EPSILON;
	if(bounds.z < FLT_EPSILON)
		bounds.z = FLT_EPSILON;

	return Containers::BoundingBox(bounds, transform.Position());
}
} // namespace LibRay