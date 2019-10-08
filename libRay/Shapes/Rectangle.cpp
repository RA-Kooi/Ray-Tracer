#include "Rectangle.hpp"

#include <cmath>
#include <float.h>

#include "../Containers/BoundingBox.hpp"
#include "../Math/MathUtils.hpp"
#include "../Math/Matrix.hpp"
#include "../Math/Ray.hpp"
#include "../Intersection.hpp"

using namespace LibRay::Materials;
using namespace LibRay::Math;

namespace LibRay::Shapes
{
std::optional<Intersection> Rectangle::Intersects(Ray const &ray) const
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

		Vector3 const pointOnRect = modelRay.Origin()
			+ modelRay.Direction()
			* distanceToCenter;

		// If all the signs are the same, the point is on the rectangle
		if(pointOnRect.x > -0.5f && pointOnRect.x < 0.5f
		   && pointOnRect.y > -0.5f && pointOnRect.y < 0.5f)
		{
			return Intersection(
				*this,
				Transform::TransformDirection(transform.Matrix(), normal),
				Transform::TransformTranslation(transform.Matrix(), pointOnRect),
				{pointOnRect.x + 0.5f, pointOnRect.y + 0.5f});
		}
	}

	return std::nullopt;
}

Containers::BoundingBox Rectangle::CalculateBoundingBox() const
{
	Vector3 const topLeft(-0.5f, 0.5f, 0);
	Vector3 const topRight(0.5f, 0.5f, 0);
	Vector3 const bottomLeft(-0.5f, -0.5f, 0);
	Vector3 const bottomRight(0.5f, -0.5f, 0);

	Vector3 const worldTopLeft =
		Transform::TransformDirection(transform.Matrix(), topLeft);
	Vector3 const worldTopRight =
		Transform::TransformDirection(transform.Matrix(), topRight);
	Vector3 const worldBottomLeft =
		Transform::TransformDirection(transform.Matrix(), bottomLeft);
	Vector3 const worldBottomRight =
		Transform::TransformDirection(transform.Matrix(), bottomRight);

	Vector3 const min =
		glm::min(worldTopLeft,
		glm::min(worldTopRight,
		glm::min(worldBottomLeft, worldBottomRight)));

	Vector3 const max =
		glm::max(worldTopLeft,
		glm::max(worldTopRight,
		glm::max(worldBottomLeft, worldBottomRight)));

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
