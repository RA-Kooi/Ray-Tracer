#include "Disc.hpp"

#include <float.h>

#include "../Containers/BoundingBox.hpp"
#include "../Math/Matrix.hpp"
#include "../Math/Ray.hpp"
#include "../Intersection.hpp"

using namespace LibRay::Materials;
using namespace LibRay::Math;

namespace LibRay::Shapes
{
std::optional<Intersection> Disc::Intersects(Ray const &ray) const
{
	Matrix4x4 const worldToModel = transform.InverseMatrix();

	Ray const modelRay(
		Transform::TransformTranslation(worldToModel, ray.Origin()),
		Transform::TransformDirection(worldToModel, ray.Direction()));

	Vector3 const normal = Vector3(0, 0, 1);

	Vector3 const centerToOrigin = modelRay.Origin();
	float const denominator = glm::dot(normal, modelRay.Direction());

	if(-denominator > FLT_EPSILON)
	{
		float const distanceToCenter = glm::dot(centerToOrigin, normal)
			/ -denominator;

		Vector3 const pointOnDisc = modelRay.Origin()
			+ modelRay.Direction()
			* distanceToCenter;

		float const squareDistance = glm::length2(pointOnDisc);

		if(squareDistance <= 1.f)
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
	Vector3 const xMin(-1.f, 0, 0);
	Vector3 const xMax(1.f, 0 , 0);

	Vector3 const yMin(0, -1.f, 0);
	Vector3 const yMax(0, 1.f, 0);

	Vector3 const zMin(0, 0, -1.f);
	Vector3 const zMax(0, 0, 1.f);

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

	Vector3 const minX = glm::min(worldXMin, worldXMax);
	Vector3 const maxX = glm::max(worldXMin, worldXMax);

	Vector3 const minY = glm::min(worldYMin, worldYMax);
	Vector3 const maxY = glm::max(worldYMin, worldYMax);

	Vector3 const minZ = glm::min(worldZMin, worldZMax);
	Vector3 const maxZ = glm::max(worldZMin, worldZMax);

	Vector3 const min = glm::min(minX, glm::min(minY, minZ));
	Vector3 const max = glm::max(maxX, glm::max(maxY, maxZ));

	Vector3 bounds = glm::abs(max - min);
	if(bounds.x < FLT_EPSILON)
		bounds.x = FLT_EPSILON;
	if(bounds.y < FLT_EPSILON)
		bounds.y = FLT_EPSILON;
	if(bounds.z < FLT_EPSILON)
		bounds.z = FLT_EPSILON;

	return Containers::BoundingBox(bounds, transform.Position());
}
} // namespace LibRay::Shapes
