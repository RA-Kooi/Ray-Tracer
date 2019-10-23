#include "Shape.hpp"

namespace LibRay::Shapes
{
using namespace LibRay::Materials;
using namespace LibRay::Math;

Shape::Shape(
	class Transform const &transform,
	MaterialStore const &materialStore,
	MaterialStore::IndexType materialIndex)
: BaseShape()
, transform(transform)
, materialStore(materialStore)
, materialIndex(materialIndex)
{
}

Shape::~Shape() noexcept = default;

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

Vector3 const Shape::PositionInternal() const
{
	return transform.Position();
}

bool Shape::IsBoundableInternal() const
{
	return true;
}
} // namespace LibRay::Shapes
