#include "Triangle.hpp"

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
std::optional<Intersection> Triangle::Intersects(Math::Ray const &ray) const
{
	// o + t * d = v0 + beta * (v1-v0) + gamma * (v2-v0)
	// -t * d +  beta * (v1-v0) + gamma * (v2-v0) = o - v0
	// M * (-t, beta, gamma) = o - v0
	// (-t, beta, gamma) = inverse(M) * (o-v0)
	// M = (d, v1-v0, v2-v0)
	// u =  inverse(M) * (o-v0)
	// t = -u.x, beta = u.y, gamma = u.z
	// there's an isect if t > 0 && 0 < beta && 0 < gamma && beta + gamma < 1

	Matrix4x4 const worldToModel = transform.InverseMatrix();
	Ray const modelRay(
		Transform::TransformTranslation(worldToModel, ray.Origin()),
		Transform::TransformDirection(worldToModel, ray.Direction()));

	Vector3 const normal = Vector3(0, 0, 1);

	Vector3 const v0(0.0f, 0.5f, 0);
	Vector3 const v1(0.5f, -0.5f, 0);
	Vector3 const v2(-0.5f, -0.5f, 0);

	Matrix3x3 const M(modelRay.Direction(), v1 - v0, v2 - v0);

	Vector3 const u = glm::inverse(M) * (modelRay.Origin() - v0);

	float const distance = -u.x;
	float const beta = u.y;
	float const gamma = u.z;

	if(distance >= 0.f && beta >= 0.f && gamma >= 0.f && beta + gamma < 1.f)
	{
		Vector3 const pos = modelRay.Origin()
			+ modelRay.Direction()
			* distance;

		Matrix4x4 const &matrix = transform.Matrix();

		return Intersection(
			*this,
			Transform::TransformDirection(matrix, normal),
			Transform::TransformTranslation(matrix, pos),
			{pos.x + 0.5f, pos.y + 0.5f});
	}

	return std::nullopt;
}

Containers::BoundingBox Triangle::CalculateBoundingBox() const
{
	Vector3 const v0(0.0f, 0.5f, 0);
	Vector3 const v1(0.5f, -0.5f, 0);
	Vector3 const v2(-0.5f, -0.5f, 0);

	Matrix4x4 const &matrix = transform.Matrix();

	Vector3 const worldV0 = matrix * Vector4(v0, 1);
	Vector3 const worldV1 = matrix * Vector4(v1, 1);
	Vector3 const worldV2 = matrix * Vector4(v2, 1);

	Vector3 const min = glm::min(worldV0, glm::min(worldV1, worldV2));
	Vector3 const max = glm::max(worldV0, glm::max(worldV1, worldV2));

	Vector3 const centroid = (min + max) * 0.5f;

	return Containers::BoundingBox((max - min) * 0.5f, centroid);
}
} // namespace LibRay::Shapes
