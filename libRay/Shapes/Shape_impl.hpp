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
} // namespace LibRay::Shapes
#endif // f2d2ca35_ed47_d2fe_659c_29870db72c06
