#ifndef aef32071_a4f3_d64d_2f9b_d395b1930919
#define aef32071_a4f3_d64d_2f9b_d395b1930919

#ifdef VIM_WORKAROUND
#include "Vector3.hpp"
#endif

namespace LibRay::Math
{
inline Vector3 Vector3::Zero()
{
	return Vector3();
}

inline Vector3 Vector3::Up()
{
	return Vector3(0, 1, 0);
}

inline Vector3 Vector3::Down()
{
	return Vector3(0, -1, 0);
}

inline Vector3 Vector3::Left()
{
	return Vector3(-1, 0, 0);
}

inline Vector3 Vector3::Right()
{
	return Vector3(1, 0, 0);
}

inline Vector3 Vector3::Forward()
{
	return Vector3(0, 0, 1);
}

inline Vector3 Vector3::Backward()
{
	return Vector3(0, 0, -1);
}
} // namespace LibRay::Math

#endif // aef32071_a4f3_d64d_2f9b_d395b1930919
