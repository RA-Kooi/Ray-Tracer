#include "Model.hpp"

#include <array>

#include "../../Math/Matrix.hpp"
#include "../../Math/Ray.hpp"
#include "../../Intersection.hpp"

using namespace LibRay::Math;

namespace LibRay::Shapes
{
Model::Model(
	std::vector<ModelTriangle::Vertex> vertices,
	class Transform const &transform,
	Materials::MaterialStore const &materialStore,
	Materials::MaterialStore::IndexType materialIndex)
: Shape(transform, materialStore, materialIndex)
, triangles()
, bvh(Load(std::move(vertices)))
{
}

std::optional<Intersection> Model::Intersects(Math::Ray const &ray) const
{
	Matrix4x4 const &worldToModel = Transform().InverseMatrix();

	Ray const modelRay(
		Transform::TransformTranslation(worldToModel, ray.Origin()),
		Transform::TransformDirection(worldToModel, ray.Direction()));

	return bvh.Traverse(modelRay);
}

Containers::BoundingBox Model::CalculateBoundingBox() const
{
	Vector3 const &rootBB = bvh.RootBoundingBox().HalfBoundaries();

	Vector3 const frontBottomLeft(-rootBB.x, -rootBB.y, -rootBB.z);
	Vector3 const frontBottomRight(rootBB.x, -rootBB.y, -rootBB.x);
	Vector3 const frontTopLeft(-rootBB.x, rootBB.y, -rootBB.z);
	Vector3 const frontTopRight(rootBB.x, rootBB.y, -rootBB.z);

	Vector3 const backBottomLeft(-rootBB.x, -rootBB.y, rootBB.z);
	Vector3 const backBottomRight(rootBB.x, -rootBB.y, rootBB.z);
	Vector3 const backTopLeft(-rootBB.x, rootBB.y, rootBB.z);
	Vector3 const backTopRight(rootBB.x, rootBB.y, rootBB.z);

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

	return Containers::BoundingBox(
		(max - min) * 0.5f,
		transform.Position() + bvh.RootBoundingBox().Position());
}

std::vector<Observer<BaseShape<ModelTriangle> const>>
Model::Load(std::vector<ModelTriangle::Vertex> vertices)
{
	std::size_t const vertexCount = vertices.size();

	std::vector<Observer<BaseShape<ModelTriangle> const>> bvhTris;
	bvhTris.reserve(vertexCount / 3);
	triangles.reserve(vertexCount / 3);

	for(size_t i = 0, j = 0; i < vertexCount; i += 3, ++j)
	{
		ModelTriangle::Vertex const &v0 = vertices[i + 0];
		ModelTriangle::Vertex const &v1 = vertices[i + 1];
		ModelTriangle::Vertex const &v2 = vertices[i + 2];

		std::array<ModelTriangle::Vertex, 3> verts{v0, v1, v2};

		triangles.emplace_back(this, std::move(verts));
		bvhTris.push_back(&triangles[j]);
	}

	std::printf("Tris: %zu\n", triangles.size());

	std::fflush(stdout);

	return bvhTris;
}
} // namespace LibRay::Shapes
