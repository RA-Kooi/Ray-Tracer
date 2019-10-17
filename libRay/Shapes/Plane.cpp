#include "Plane.hpp"

#include <cfloat>

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

		Vector3 const localPoint = modelRay.Origin()
				+ modelRay.Direction()
				* distanceToIntersection;

		Vector2 const uv(
			std::fmod(localPoint.x / 10.f, 100.f),
			-std::fmod(localPoint.z / 10.f, 100.f));

		Matrix4x4 const &matrix = transform.Matrix();

		Vector3 const pointOnPlane =
			Transform::TransformTranslation(matrix, localPoint);

		Vector3 const translatedNormal = Transform::TransformDirection(
			matrix,
			normal);

		return Intersection(*this, translatedNormal, pointOnPlane, uv);
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
