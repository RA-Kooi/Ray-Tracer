#include "Rectangle.hpp"

#include <cmath>
#include <float.h>

#include "../Containers/BoundingBox.hpp"
#include "../Math/MathUtils.hpp"
#include "../Math/Ray.hpp"
#include "../Intersection.hpp"

using namespace LibRay::Math;

namespace LibRay::Shapes
{
Rectangle::	Rectangle(
	Math::Vector2 const &dimensions,
	class Transform const &transform,
	MaterialStore const &materialStore,
	MaterialStore::IndexType materialIndex,
	class Color const &color)
: Shape(transform, materialStore, materialIndex, color)
, dimensions(dimensions)
{
}

std::optional<Intersection> Rectangle::Intersects(Ray const &ray) const
{
	Matrix4x4 const worlToModel = transform.InverseMatrix();

	Ray const modelRay(
		Transform::TransformTranslation(worlToModel, ray.Origin()),
		Transform::TransformDirection(worlToModel, ray.Direction()));

	Vector3 const normal = Vector3::Forward();
	Vector3 const centerToOrigin = modelRay.Origin();

	float const denominator = normal.Dot(modelRay.Direction());
	if(-denominator > FLT_EPSILON)
	{
		float const distanceToCenter = centerToOrigin.Dot(normal)
			/ -denominator;

		Vector3 const pointOnRect = modelRay.Origin()
			+ modelRay.Direction()
			* distanceToCenter;

		// Determine the vertices of the rectangle
		float const halfWidth = dimensions.x * 0.5f;
		float const halfHeight = dimensions.y * 0.5f;

		// If all the signs are the same, the point is on the rectangle
		if(pointOnRect.x > -halfWidth && pointOnRect.x < halfWidth
		   && pointOnRect.y > -halfHeight && pointOnRect.y < halfHeight)
		{
			return Intersection(
				*this,
				Transform::TransformDirection(transform.Matrix(), normal),
				Transform::TransformTranslation(transform.Matrix(), pointOnRect));
		}
	}

	return std::nullopt;
}

Containers::BoundingBox Rectangle::CalculateBoundingBox() const
{
	Vector3 const topLeft(-dimensions.x, dimensions.y);
	Vector3 const topRight(dimensions.x, dimensions.y);
	Vector3 const BottomLeft(-dimensions.x, -dimensions.y);
	Vector3 const BottomRight(dimensions.x, -dimensions.y);

	Vector3 const worldTopLeft =
		Transform::TransformDirection(transform.Matrix(), topLeft);
	Vector3 const worldTopRight =
		Transform::TransformDirection(transform.Matrix(), topRight);
	Vector3 const worldBottomLeft =
		Transform::TransformDirection(transform.Matrix(), topLeft);
	Vector3 const worldBottomRight =
		Transform::TransformDirection(transform.Matrix(), topRight);

	Vector3 const min = worldTopLeft
		.Min(worldTopRight)
		.Min(worldBottomLeft)
		.Min(worldBottomRight);

	Vector3 const max = worldTopLeft
		.Max(worldTopRight)
		.Max(worldBottomLeft)
		.Max(worldBottomRight);

	Vector3 bounds = (max - min).Abs();
	if(bounds.x < FLT_EPSILON)
		bounds.x = FLT_EPSILON;
	if(bounds.y < FLT_EPSILON)
		bounds.y = FLT_EPSILON;
	if(bounds.z < FLT_EPSILON)
		bounds.z = FLT_EPSILON;

	return Containers::BoundingBox(bounds, transform.Position());
}
} // namespace LibRay::Shapes
