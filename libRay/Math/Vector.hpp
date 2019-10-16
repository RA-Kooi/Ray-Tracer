#ifndef f51e2d4a_b54f_2fdf_a142_ebc7a4bcf3a3
#define f51e2d4a_b54f_2fdf_a142_ebc7a4bcf3a3

#include <cstddef>
#include <ostream>

#include <glm/gtx/extended_min_max.hpp>
#include <glm/gtx/component_wise.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/geometric.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

namespace LibRay::Math
{
using Vector2 = glm::vec2;
using Vector2st = glm::vec<2, std::size_t, glm::highp>;
using Vector3 = glm::vec3;
using Vector4 = glm::vec4;
} // namespace LibRay::Math

inline std::ostream &operator<<(std::ostream &o, LibRay::Math::Vector4 const &v);
inline std::ostream &operator<<(std::ostream &o, LibRay::Math::Vector4 const &v)
{
	return o
		<< "[X: " << v.x
		<< ", Y: " << v.y
		<<  ", Z: " << v.z
		<< ", W: " << v.w
		<< "]";
}

inline std::ostream &operator<<(std::ostream &o, LibRay::Math::Vector3 const &v);
inline std::ostream &operator<<(std::ostream &o, LibRay::Math::Vector3 const &v)
{
	return o << "[X: " << v.x << ", Y: " << v.y <<  ", Z: " << v.z << "]";
}

inline std::ostream &operator<<(std::ostream &o, LibRay::Math::Vector2 const &v);
inline std::ostream &operator<<(std::ostream &o, LibRay::Math::Vector2 const &v)
{
	return o << "[X: " << v.x << ", Y: " << v.y <<  "]";
}

inline std::ostream &operator<<(std::ostream &o, LibRay::Math::Vector2st const &v);
inline std::ostream &operator<<(std::ostream &o, LibRay::Math::Vector2st const &v)
{
	return o << "[X: " << v.x << ", Y: " << v.y <<  "]";
}

#endif // f51e2d4a_b54f_2fdf_a142_ebc7a4bcf3a3
