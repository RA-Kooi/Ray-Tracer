#include "MaterialStore.hpp"

#include <algorithm>
#include <cassert>
#include <stdexcept>

namespace LibRay::Materials
{
MaterialStore::IndexType MaterialStore::AddMaterial(
	std::string const &name,
	Material material)
{
	auto const it = std::find_if(
		materials.cbegin(),
		materials.cend(),
		[&name](std::pair<std::string, Material> const &value)
		{
			return name == value.first;
		});

	if(it != materials.cend())
		throw std::runtime_error(
			"Material with name <" + name + "> already exists!\n");

	IndexType index = materials.size();

	materials.emplace_back(name, std::move(material));

	return index;
}

Material const &MaterialStore::MaterialByIndex(IndexType index) const
{
	assert(index < materials.size());

	return materials[index].second;
}

MaterialStore::IndexType MaterialStore::MaterialIndexByName(
	std::string const &name) const
{
	for(std::size_t i = 0; i < materials.size(); ++i)
	{
		if(materials[i].first == name)
			return i;
	}

	throw std::runtime_error(
		"Material with name <" + name + "> doesn't exist!\n");
}
} // namespace LibRay::Materials
