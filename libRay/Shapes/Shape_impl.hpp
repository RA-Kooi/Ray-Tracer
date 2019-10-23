#ifndef f2d2ca35_ed47_d2fe_659c_29870db72c06
#define f2d2ca35_ed47_d2fe_659c_29870db72c06

#ifdef VIM_WORKAROUND
#include "Shape.hpp"
#endif

namespace LibRay::Shapes
{
using namespace LibRay::Materials;
using namespace LibRay::Math;

template<typename T>
T const &BaseShape<T>::Derived() const
{
	return static_cast<T const &>(*this);
}

template<typename T>
Vector3 const BaseShape<T>::Position() const
{
	return Derived().PositionInternal();
}

template<typename T>
BaseShape<T>::~BaseShape() noexcept = default;

template<typename T>
bool BaseShape<T>::IsBoundable() const
{
	return true;
}

template<typename T>
std::optional<Intersection> BaseShape<T>::Intersects(Ray const &ray) const
{
	return Derived().IntersectsInternal(ray);
}

template<typename T>
Containers::BoundingBox BaseShape<T>::CalculateBoundingBox() const
{
	return Derived().CalculateBoundingBoxInternal();
}
} // namespace LibRay::Shapes
#endif // f2d2ca35_ed47_d2fe_659c_29870db72c06
