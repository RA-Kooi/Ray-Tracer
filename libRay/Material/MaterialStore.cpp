#include "MaterialStore.hpp"

#include <cassert>

namespace LibRay
{
MaterialStore::IndexType MaterialStore::AddMaterial(Material &&material)
{
	IndexType index = materials.size();

	materials.push_back(material);

	return index;
}

Material const &MaterialStore::MaterialByIndex(IndexType index) const
{
	assert(index < materials.size());

	return materials[index];
}
} // namespace LibRay
