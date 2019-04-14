#ifndef d27463cf_26e0_1640_686b_b59c0923cf20
#define d27463cf_26e0_1640_686b_b59c0923cf20

#include <Eigen/Dense>

namespace LibRay
{
namespace Math
{
using Vector2 = Eigen::Vector2f;
using Vector3 = Eigen::Vector3f;
using Vector4 = Eigen::Vector4f;
} // namespace Math
} // namespace LibRay

namespace Eigen
{
inline Vector2f operator/(float a, Vector2f const &b)
{
	return Vector2f(a / b.x(), a / b.y());
}

inline Vector3f operator/(float a, Vector3f const &b)
{
	return Vector3f(a / b.x(), a / b.y(), a / b.z());
}

inline Vector4f operator/(float a, Vector4f const &b)
{
	return Vector4f(a / b.x(), a / b.y(), a / b.z(), a / b.w());
}
} // namespace Eigen

#endif // d27463cf_26e0_1640_686b_b59c0923cf20
