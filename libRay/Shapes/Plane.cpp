#include "Plane.hpp"

#include <float.h>

#include "../Containers/BoundingBox.hpp"
#include "../Math/Matrix.hpp"
#include "../Math/Ray.hpp"
#include "../Math/Vector.hpp"
#include "../Intersection.hpp"
#include "../Transform.hpp"

using namespace LibRay::Math;

namespace LibRay::Shapes
{
std::optional<Intersection> Plane::Intersects(Ray const &ray) const
{
	Matrix4x4 const worldToModel = transform.InverseMatrix();
	Ray const modelRay(
		Transform::TransformTranslation(worldToModel, ray.Origin()),
		Transform::TransformDirection(worldToModel, ray.Direction()));

	Vector3 const normal = Vector3(0, 1, 0);

	Vector3 const centerToOrigin = modelRay.Origin();
	float const denominator = glm::dot(normal, modelRay.Direction());

	if(-denominator > FLT_EPSILON)
	{
		float const distanceToIntersection = glm::dot(centerToOrigin, normal)
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
	return Containers::BoundingBox(Vector3(0), Vector3(0));
}
} // namespace LibRay::Shapes
