#include "ModelLoader.hpp"

#include <algorithm>
#include <numeric>
#include <stdexcept>
#include <unordered_map>

#include <tinyobjloader/tiny_obj_loader.hpp>

#include "../../Math/Vector.hpp"

using namespace LibRay::Materials;
using namespace LibRay::Math;

namespace LibRay::Shapes
{
ModelLoader::ModelLoader(MaterialStore &materialStore)
: materialStore(materialStore)
{
}

std::vector<std::unique_ptr<Model>> ModelLoader::LoadObj(
	std::string const &fileName,
	Transform const &transform,
	std::string const &materialDir,
	MaterialStore::IndexType materialIndex,
	bool invertNormalZ) const
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
		fileName.c_str(),
		materialDir.c_str());

	if(!result)
	{
		throw std::runtime_error(
			"Failed to load <" + fileName + ">\n"
			+ warnings + "\n"
			+ errors + "\n");
	}

	std::printf(
		"Raw vertex count: %zu, raw texcoord count: %zu, raw normal count: %zu\n"
		"Shape count: %zu, material count: %zu\n",
		attributes.vertices.size(),
		attributes.texcoords.size(),
		attributes.normals.size(),
		shapes.size(),
		materials.size());

	std::vector<std::unique_ptr<Model>> models;
	models.reserve(shapes.size());

	for(size_t i = 0; i < shapes.size(); ++i)
	{
		std::printf("Shape %zu: %s\n", i, shapes[i].name.c_str());
	}

	for(size_t i = 0; i < materials.size(); ++i)
	{
		std::printf("Material %zu: %s\n", i, materials[i].name.c_str());
	}

	std::fflush(stdout);

	for(tinyobj::shape_t const &shape: shapes)
	{
		bool skip = false;
		for(size_t i = 0; i < shape.mesh.num_face_vertices.size(); ++i)
		{
			if(shape.mesh.num_face_vertices[i] != 3)
			{
				skip = true;
				break;
			}
		}

		if(skip)
		{
			std::fprintf(
				stderr,
				"Shape with name %s has unsupported face configuration, "
				"skipping...\n",
				shape.name.c_str());

			continue;
		}

		if(shape.mesh.material_ids[0] >= 0)
		{
			materialIndex =
				materialStore.MaterialIndexByName(
					materials[size_t(shape.mesh.material_ids[0])].name);
		}

		std::vector<tinyobj::index_t> const &indices = shape.mesh.indices;

		std::vector<ModelTriangle::Vertex> vertices;
		vertices.reserve(indices.size());

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

		models.push_back(
			std::make_unique<Model>(
				std::move(vertices),
				transform,
				materialStore,
				materialIndex));
	}

	return models;
}
} // namespace LibRay::Shapes
