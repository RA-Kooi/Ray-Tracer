#include "Shape.hpp"

using namespace LibRay::Materials;
using namespace LibRay::Math;

namespace LibRay::Shapes
{
Shape::Shape(
	class Transform const &transform,
	MaterialStore const &materialStore,
	MaterialStore::IndexType materialIndex,
	class Color const &color)
: color(color)
, materialStore(materialStore)
, materialIndex(materialIndex)
, transform(transform)
{
}

Color const &Shape::Color() const
{
	return color;
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
