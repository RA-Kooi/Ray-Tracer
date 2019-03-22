#include "Disc.hpp"

#include <float.h>

#include "../Containers/BoundingBox.hpp"
#include "../Math/Ray.hpp"
#include "../Intersection.hpp"

using namespace LibRay::Math;

namespace LibRay::Shapes
{
Disc::Disc(
	float radius,
	class Transform const &transform,
	MaterialStore const &materialStore,
	MaterialStore::IndexType materialIndex,
	class Color const &color)
: Shape(transform, materialStore, materialIndex, color)
, radius(radius)
{
}

std::optional<Intersection> Disc::Intersects(Ray const &ray) const
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

		Vector3 const pointOnDisc = modelRay.Origin()
			+ modelRay.Direction()
			* distanceToCenter;

		float const squareDistance = pointOnDisc.SquareMagnitude();

		if(squareDistance <= radius * radius)
		{
			return Intersection(
				*this,
				Transform::TransformDirection(transform.Matrix(), normal),
				Transform::TransformTranslation(transform.Matrix(), pointOnDisc));
		}
	}

	return std::nullopt;
}

Containers::BoundingBox Disc::CalculateBoundingBox() const
{
	Vector3 const xMin(-radius);
	Vector3 const xMax(radius);

	Vector3 const yMin(0, -radius);
	Vector3 const yMax(0, radius);

	Vector3 const zMin(0, 0, -radius);
	Vector3 const zMax(0, 0, radius);

	Vector3 const worldXMin =
		Transform::TransformDirection(transform.Matrix(), xMin);
	Vector3 const worldXMax =
		Transform::TransformDirection(transform.Matrix(), xMax);

	Vector3 const worldYMin =
		Transform::TransformDirection(transform.Matrix(), yMin);
	Vector3 const worldYMax =
		Transform::TransformDirection(transform.Matrix(), yMax);

	Vector3 const worldZMin =
		Transform::TransformDirection(transform.Matrix(), zMin);
	Vector3 const worldZMax =
		Transform::TransformDirection(transform.Matrix(), zMax);

	Vector3 const minX = worldXMin.Min(worldXMax);
	Vector3 const maxX = worldXMin.Max(worldXMax);

	Vector3 const minY = worldYMin.Min(worldYMax);
	Vector3 const maxY = worldYMin.Max(worldYMax);

	Vector3 const minZ = worldZMin.Min(worldZMax);
	Vector3 const maxZ = worldZMin.Max(worldZMax);

	Vector3 const min = minX.Min(minY).Min(minZ);
	Vector3 const max = maxX.Max(maxY).Max(maxZ);

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
