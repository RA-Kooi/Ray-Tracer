#ifndef d12df214_81ec_afa6_c2a0_8b4d710db950
#define d12df214_81ec_afa6_c2a0_8b4d710db950

#ifdef VIM_WORKAROUND
#include "Vector2.hpp"
#endif

namespace LibRay::Math
{
inline Vector2 Vector2::Zero()
{
	return Vector2();
}

inline Vector2 Vector2::Up()
{
	return Vector2(0, 1);
}

inline Vector2 Vector2::Down()
{
	return Vector2(0, -1);
}

inline Vector2 Vector2::Left()
{
	return Vector2(-1, 0);
}

inline Vector2 Vector2::Right()
{
	return Vector2(1, 0);
}
} // namespace LibRay::Math

#endif // d12df214_81ec_afa6_c2a0_8b4d710db950
