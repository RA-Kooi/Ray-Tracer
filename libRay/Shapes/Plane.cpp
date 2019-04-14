#include "Plane.hpp"

#include <float.h>

#include "../Containers/BoundingBox.hpp"
#include "../Math/Ray.hpp"
#include "../Intersection.hpp"
#include "../Transform.hpp"

using namespace LibRay::Math;

namespace LibRay::Shapes
{
boost::optional<Intersection> Plane::Intersects(Ray const &ray) const
{
	Matrix4x4 const worlToModel = transform.InverseMatrix();

	Ray const modelRay(
		Transform::TransformTranslation(worlToModel, ray.Origin()),
		Transform::TransformDirection(worlToModel, ray.Direction()));

	Vector3 const normal = Vector3(0, 1, 0);

	Vector3 const centerToOrigin = modelRay.Origin();
	float const denominator = normal.dot(modelRay.Direction());

	if(-denominator > FLT_EPSILON)
	{
		float const distanceToIntersection = centerToOrigin.dot(normal)
			/ -denominator;

		Vector3 const pointOnPlane = Transform::TransformTranslation(
			transform.Matrix(),
			modelRay.Origin()
				+ modelRay.Direction()
				* distanceToIntersection);

		Vector3 const translatedNormal = Transform::TransformDirection(
			transform.Matrix(),
			normal);

		return Intersection(*this, translatedNormal, pointOnPlane);
	}

	return boost::none;
}

bool Plane::IsBoundable() const
{
	return false;
}

Containers::BoundingBox Plane::CalculateBoundingBox() const
{
	return Containers::BoundingBox(Vector3::Zero(), Vector3::Zero());
}
} // namespace LibRay::Shapes
