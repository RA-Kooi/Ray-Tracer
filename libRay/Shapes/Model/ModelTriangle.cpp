#include "ModelTriangle.hpp"

#include <cfloat>

#include "../../Containers/BoundingBox.hpp"
#include "../../Math/Matrix.hpp"
#include "../../Math/Ray.hpp"
#include "../../Math/Vector.hpp"
#include "../../Intersection.hpp"
#include "Model.hpp"

using namespace LibRay::Math;

namespace LibRay::Shapes
{
ModelTriangle::ModelTriangle(
	Observer<Model const> parent,
	std::array<Vertex, 3> vertices)
: BaseShape()
, parent(parent)
, vertices(std::move(vertices))
{
	// Compute plane normal
	Vector3 const v1v0 = vertices[1].position - vertices[0].position;
	Vector3 const v2v0 = vertices[2].position - vertices[0].position;

	// Compute UV normal
	Vector2 const u1u0 = vertices[1].uv - vertices[0].uv;
	Vector2 const u2u0 = vertices[2].uv - vertices[0].uv;

	// Compute inverse cross product of the UV
	float const r = 1.0f / (u1u0.x * u2u0.y - u1u0.y * u2u0.x);

	tangent = (v1v0 * u2u0.y - v2v0 * u1u0.y) * r;
}

std::optional<Intersection> ModelTriangle::IntersectsInternal(
	Ray const &modelRay) const
{
	// Compute plane normal
	Vector3 const v1v0 = vertices[1].position - vertices[0].position;
	Vector3 const v2v0 = vertices[2].position - vertices[0].position;

	Matrix3x3 const M(modelRay.Direction(), v1v0, v2v0);

	Vector3 const u = glm::inverse(M)
		* (modelRay.Origin() - vertices[0].position);

	float const distance = -u.x;
	float const beta = u.y;
	float const gamma = u.z;

	if(distance >= 0.f && beta >= 0.f && gamma >= 0.f && beta + gamma < 1.f)
	{
		Vector3 const pos = modelRay.Origin()
			+ modelRay.Direction()
			* distance;

		float const alpha = (1.f - beta - gamma);

		Vector3 const normal =
			vertices[0].normal * alpha
			+ vertices[1].normal * beta
			+ vertices[2].normal * gamma;

		Vector2 const uv = (
			vertices[0].uv * alpha
			+ vertices[1].uv * beta
			+ vertices[2].uv * gamma);

		Matrix4x4 const &matrix = parent->Transform().Matrix();

		return Intersection(
			*parent,
			Transform::TransformDirection(matrix, normal),
			Transform::TransformDirection(matrix, tangent),
			Transform::TransformTranslation(matrix, pos),
			uv);
	}

	return std::nullopt;
}

Containers::BoundingBox ModelTriangle::CalculateBoundingBoxInternal() const
{
	Vector3 const min = glm::min(
		vertices[0].position,
		glm::min(vertices[1].position, vertices[2].position));

	Vector3 const max = glm::max(
		vertices[0].position,
		glm::max(vertices[1].position, vertices[2].position));

	Vector3 const centroid = (min + max) * 0.5f;

	return Containers::BoundingBox((max - min) * 0.5f, centroid);
}

Vector3 const ModelTriangle::PositionInternal() const
{
	constexpr float const inv3 = 1.f / 3.f;

	return (
		vertices[0].position
		+ vertices[1].position
		+ vertices[2].position
		) * inv3;
}
} // namespace LibRay::Shapes
