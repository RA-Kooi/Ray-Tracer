#include "Plane.hpp"

#include <float.h>

#include "../Containers/BoundingBox.hpp"
#include "../Math/Ray.hpp"
#include "../Intersection.hpp"
#include "../Transform.hpp"

using namespace LibRay::Math;

namespace LibRay::Shapes
{
std::optional<Intersection> Plane::Intersects(Ray const &ray) const
{
	Matrix4x4 worlToModel = transform.InverseMatrix();
	Ray const modelRay(
		Transform::TransformTranslation(worlToModel, ray.Origin()),
		Transform::TransformDirection(worlToModel, ray.Direction()));

	Vector3 const normal = Vector3::Up();

	Vector3 const centerToOrigin = modelRay.Origin();
	float const denominator = normal.Dot(modelRay.Direction());

	if(-denominator > FLT_EPSILON)
	{
		float const distanceToIntersection = centerToOrigin.Dot(normal)
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

	return std::nullopt;
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
