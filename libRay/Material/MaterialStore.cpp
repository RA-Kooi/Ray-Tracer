#include "MaterialStore.hpp"

#include <cassert>

namespace LibRay::Materials
{
MaterialStore::IndexType MaterialStore::AddMaterial(Material material)
{
	IndexType index = materials.size();

	materials.push_back(std::move(material));

	return index;
}

Material const &MaterialStore::MaterialByIndex(IndexType index) const
{
	assert(index < materials.size());

	return materials[index];
}
} // namespace LibRay::Materials
