#include "Model.hpp"

#include <algorithm>
#include <array>
#include <numeric>
#include <stdexcept>

#include <tinyobjloader/tiny_obj_loader.hpp>

#include "../../Math/Matrix.hpp"
#include "../../Math/Ray.hpp"
#include "../../Intersection.hpp"

using namespace LibRay::Math;

namespace LibRay::Shapes
{
Model::Model(
	std::string const &fileName,
	bool invertNormalZ,
	class Transform const &transform,
	Materials::MaterialStore const &materialStore,
	Materials::MaterialStore::IndexType materialIndex)
: Shape(transform, materialStore, materialIndex)
, triangles()
, bvh(Load(fileName, invertNormalZ))
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
Model::Load(std::string const &fileName, bool invertNormalZ)
{
	tinyobj::attrib_t attributes;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;

	std::string warnings, errors;

	bool const result = tinyobj::LoadObj(
		&attributes,
		&shapes,
		&materials,
		&warnings,
		&errors,
		fileName.c_str());

	if(!result)
	{
		throw std::runtime_error(
			"Failed to load <" + fileName + ">\n"
			+ warnings + "\n"
			+ errors + "\n");
	}

	std::printf(
		"Raw vertex count: %zu, raw texcoord count: %zu, raw normal count: %zu\n",
		attributes.vertices.size(),
		attributes.texcoords.size(),
		attributes.normals.size());

	size_t const indexCount = std::accumulate(
		shapes.cbegin(),
		shapes.cend(),
		size_t(0),
		[](size_t currentTotal, tinyobj::shape_t const &currentShape)
		{
			return currentTotal + currentShape.mesh.indices.size();
		});

	std::printf(
		"Indices: %zu, shapes: %zu\n",
		indexCount,
		shapes.size());

	// Assert we get triangles
	assert(lldiv(static_cast<std::make_signed_t<size_t>>(indexCount), 3).rem == 0);

	std::vector<ModelTriangle::Vertex> vertices;
	vertices.reserve(indexCount);

	for(tinyobj::shape_t const &shape: shapes)
	{
		std::vector<tinyobj::index_t> const &indices = shape.mesh.indices;

		std::generate_n(
			std::back_inserter(vertices),
			indices.size(),
			[i = size_t(0), &indices, &attributes, invertNormalZ]() mutable
			{
				std::vector<tinyobj::real_t> const &verts = attributes.vertices;
				std::vector<tinyobj::real_t> const &normals = attributes.normals;
				std::vector<tinyobj::real_t> const &texCoords = attributes.texcoords;

				ModelTriangle::Vertex vertex;

				int signedIndex = indices[i].vertex_index;

				if(signedIndex < 0)
					signedIndex = int(verts.size()) - signedIndex;

				size_t index = size_t(signedIndex) * 3;

				vertex.position = Vector3(
					verts[index + 0],
					verts[index + 1],
					verts[index + 2]);

				signedIndex = indices[i].normal_index;

				if(signedIndex < 0)
					signedIndex = int(verts.size()) - signedIndex;

				index = size_t(signedIndex) * 3;

				vertex.normal = Vector3(
					normals[index + 0],
					normals[index + 1],
					invertNormalZ ? -normals[index + 2] : normals[index + 2]);

				if(texCoords.size() > 0)
				{
					signedIndex = indices[i].texcoord_index;

					if(signedIndex < 0)
						signedIndex = int(verts.size()) - signedIndex;

					index = size_t(signedIndex) * 2;

					vertex.uv = Vector2(
						texCoords[index + 0],
						texCoords[index + 1]);
				}
				else
					vertex.uv = Vector2(0);

				++i;

				return vertex;
			});
	}

	assert(vertices.size() == indexCount);

	std::vector<Observer<BaseShape<ModelTriangle> const>> bvhTris;
	bvhTris.reserve(indexCount / 3);
	triangles.reserve(indexCount / 3);

	for(size_t i = 0, j = 0; i < indexCount; i += 3, ++j)
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
