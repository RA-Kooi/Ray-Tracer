#include "Shape.hpp"

#include "Model/ModelTriangle.hpp"

using namespace LibRay::Materials;
using namespace LibRay::Math;

namespace LibRay::Shapes
{
template<typename T>
Vector3 const BaseShape<T>::Position() const
{
	return static_cast<T const &>(*this).PositionInternal();
}

template<typename T>
BaseShape<T>::~BaseShape() noexcept = default;

template<typename T>
bool BaseShape<T>::IsBoundable() const
{
	return true;
}

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

template class BaseShape<Shape>;
template class BaseShape<ModelTriangle>;
} // namespace LibRay::Shapes
