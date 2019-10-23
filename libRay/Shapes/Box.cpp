#include "Box.hpp"

#include <algorithm>

#include "../Containers/BoundingBox.hpp"
#include "../Math/MathUtils.hpp"
#include "../Math/Matrix.hpp"
#include "../Math/Ray.hpp"
#include "../Math/Vector.hpp"
#include "../Intersection.hpp"
#include "../Transform.hpp"

namespace LibRay
{
using namespace Math;
using namespace Shapes;

static Vector3 CalculateNormal(Vector3 const &point)
{
	Vector3 const pointAbs = glm::abs(point);
	Vector3 normal(0);

	if(pointAbs.x >= pointAbs.y && pointAbs.x >= pointAbs.z)
		normal.x = float(Sign(point.x));

	if(pointAbs.y >= pointAbs.x && pointAbs.y >= pointAbs.z)
		normal.y = float(Sign(point.y));

	if(pointAbs.z >= pointAbs.x && pointAbs.z >= pointAbs.y)
		normal.z = float(Sign(point.z));

	if(normal.x > 0 && normal.y > 0)
		normal.y = 0;
	if(normal.x > 0 && normal.z > 0)
		normal.z = 0;
	if(normal.y > 0 && normal.z > 0)
		normal.z = 0;

	return normal;
}

static Vector2 UV(Vector3 const &modelPos, Vector3 const &normal)
{
	if(normal.x != 0.f)
		return {-modelPos.z + 0.5f, -modelPos.y + 0.5f};
	else if(normal.y != 0.f)
		return {modelPos.x + 0.5f, modelPos.z + 0.5f};
	else
		return {modelPos.x + 0.5f, -modelPos.y + 0.5f};
}

std::optional<Intersection> Box::IntersectsInternal(Math::Ray const &ray) const
{
	Matrix4x4 const worldToModel = transform.InverseMatrix();
	Ray const modelRay(
		Transform::TransformTranslation(worldToModel, ray.Origin()),
		Transform::TransformDirection(worldToModel, ray.Direction()));

	Vector3 const halfBoundaries(0.5f, 0.5f, 0.5f);

	Vector3 const inverseDir = 1.f / modelRay.Direction();
	Vector3 const hit1 = (-halfBoundaries - modelRay.Origin()) * inverseDir;
	Vector3 const hit2 = (halfBoundaries - modelRay.Origin()) * inverseDir;

	Vector3 const min = glm::min(hit1, hit2);
	Vector3 const max = glm::max(hit1, hit2);

	float const near = glm::compMax(min);
	float const far = glm::compMin(max);

	if(far < near)
		return std::nullopt;

	float distanceToIntersection = near;

	if(distanceToIntersection < 0)
	{
		distanceToIntersection = far;

		if(distanceToIntersection < 0)
			return std::nullopt;
	}

	Vector3 const pointOnBox = modelRay.Origin()
		+ modelRay.Direction()
		* distanceToIntersection;

	Vector3 const normal = CalculateNormal(pointOnBox);

	Vector2 const uv = UV(pointOnBox, normal);

	return Intersection(
		*this,
		Transform::TransformDirection(transform.Matrix(), normal),
		Transform::TransformTranslation(transform.Matrix(), pointOnBox),
		uv);
}

Containers::BoundingBox Box::CalculateBoundingBoxInternal() const
{
	Vector3 const frontBottomLeft(-0.5, -0.5, -0.5);
	Vector3 const frontBottomRight(0.5, -0.5, -0.5);
	Vector3 const frontTopLeft(-0.5, 0.5, -0.5);
	Vector3 const frontTopRight(0.5, 0.5, -0.5);

	Vector3 const backBottomLeft(-0.5, -0.5, 0.5);
	Vector3 const backBottomRight(0.5, -0.5, 0.5);
	Vector3 const backTopLeft(-0.5, 0.5, 0.5);
	Vector3 const backTopRight(0.5, 0.5, 0.5);

	Matrix4x4 const &matrix = transform.Matrix();

	Vector3 const worldFrontBottomLeft =
		Transform::TransformDirection(matrix, frontBottomLeft);
	Vector3 const worldFrontBottomRight =
		Transform::TransformDirection(matrix, frontBottomRight);
	Vector3 const worldFrontTopLeft =
		Transform::TransformDirection(matrix, frontTopLeft);
	Vector3 const worldFrontTopRight =
		Transform::TransformDirection(matrix, frontTopRight);

	Vector3 const worldBackBottomLeft =
		Transform::TransformDirection(matrix, backBottomLeft);
	Vector3 const worldBackBottomRight =
		Transform::TransformDirection(matrix, backBottomRight);
	Vector3 const worldBackTopLeft =
		Transform::TransformDirection(matrix, backTopLeft);
	Vector3 const worldBackTopRight =
		Transform::TransformDirection(matrix, backTopRight);

	Vector3 const min =
		glm::min(worldFrontBottomLeft,
		glm::min(worldFrontBottomRight,
		glm::min(worldFrontTopLeft,
		glm::min(worldFrontTopRight,
		glm::min(worldBackBottomLeft,
		glm::min(worldBackBottomRight,
		glm::min(worldBackTopLeft, worldBackTopRight)))))));

	Vector3 const max =
		glm::max(worldFrontBottomLeft,
		glm::max(worldFrontBottomRight,
		glm::max(worldFrontTopLeft,
		glm::max(worldFrontTopRight,
		glm::max(worldBackBottomLeft,
		glm::max(worldBackBottomRight,
		glm::max(worldBackTopLeft, worldBackTopRight)))))));

	return Containers::BoundingBox((max - min) * 0.5f, transform.Position());
}
} // namespace LibRay
