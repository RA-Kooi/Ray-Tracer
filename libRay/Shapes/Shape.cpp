#include "Shape.hpp"

using namespace LibRay::Materials;
using namespace LibRay::Math;

namespace LibRay::Shapes
{
Shape::Shape(
	class Transform const &transform,
	MaterialStore const &materialStore,
	MaterialStore::IndexType materialIndex)
: materialStore(materialStore)
, materialIndex(materialIndex)
, transform(transform)
{
}

class Material const &Shape::Material() const
{
	return materialStore.MaterialByIndex(materialIndex);
}

Transform const &Shape::Transform() const
{
	return transform;
}

Transform &Shape::Transform()
{
	return transform;
}

bool Shape::IsBoundable() const
{
	return true;
}
} // namespace LibRay::Shapes
