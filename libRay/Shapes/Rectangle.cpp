#include "Rectangle.hpp"

#include <cfloat>
#include <cmath>

#include "../Containers/BoundingBox.hpp"
#include "../Math/MathUtils.hpp"
#include "../Math/Matrix.hpp"
#include "../Math/Ray.hpp"
#include "../Intersection.hpp"

using namespace LibRay::Materials;
using namespace LibRay::Math;

namespace LibRay::Shapes
{
std::optional<Intersection> Rectangle::IntersectsInternal(Ray const &ray) const
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
			Matrix4x4 const &matrix = transform.Matrix();

			return Intersection(
				*this,
				Transform::TransformDirection(matrix, normal),
				Transform::TransformTranslation(matrix, pointOnRect),
				{pointOnRect.x + 0.5f, pointOnRect.y + 0.5f});
		}
	}

	return std::nullopt;
}

Containers::BoundingBox Rectangle::CalculateBoundingBoxInternal() const
{
	Vector3 const topLeft(-0.5f, 0.5f, 0);
	Vector3 const topRight(0.5f, 0.5f, 0);
	Vector3 const bottomLeft(-0.5f, -0.5f, 0);
	Vector3 const bottomRight(0.5f, -0.5f, 0);

	Matrix4x4 const matrix = transform.Matrix();

	Vector3 const worldTopLeft = Transform::TransformDirection(matrix, topLeft);
	Vector3 const worldTopRight = Transform::TransformDirection(matrix, topRight);
	Vector3 const worldBottomLeft = Transform::TransformDirection(matrix, bottomLeft);
	Vector3 const worldBottomRight = Transform::TransformDirection(matrix, bottomRight);

	Vector3 const min =
		glm::min(worldTopLeft,
		glm::min(worldTopRight,
		glm::min(worldBottomLeft, worldBottomRight)));

	Vector3 const max =
		glm::max(worldTopLeft,
		glm::max(worldTopRight,
		glm::max(worldBottomLeft, worldBottomRight)));

	return Containers::BoundingBox((max - min) * 0.5f, transform.Position());
}
} // namespace LibRay::Shapes
