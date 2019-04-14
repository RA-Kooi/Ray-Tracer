#ifndef ef875083_56da_287e_58f0_a7a130757a7d
#define ef875083_56da_287e_58f0_a7a130757a7d

#include <limits>
#include <boost/optional.hpp>
#include <type_traits>
#include <vector>

#include "Math/Vector.hpp"
#include "API.hpp"
#include "Utilites.hpp"

namespace LibRay
{
namespace Math
{
class Ray;
} // namespace Math

namespace Shapes
{
class Color;
} // namespace Shapes

class Image;
class Intersection;
class Light;
class Scene;

struct LIBRAY_API RayTracerConfiguration final
{
	RayTracerConfiguration(std::uint8_t maxReflectionBounces);

	std::uint8_t maxReflectionBounces;
};

static_assert(std::is_copy_constructible_v<RayTracerConfiguration>);
static_assert(std::is_copy_assignable_v<RayTracerConfiguration>);
static_assert(std::is_trivially_copyable_v<RayTracerConfiguration>);

static_assert(std::is_move_constructible_v<RayTracerConfiguration>);
static_assert(std::is_move_assignable_v<RayTracerConfiguration>);

class LIBRAY_API RayTracer final
{
public:
	RayTracer(Scene const &scene, RayTracerConfiguration &&config);

	Image Trace() const;
	Shapes::Color TraceRay(
		Math::Ray const &ray,
		std::uint8_t reflectionBounceCount = 0,
		bool debug = false,
		float farPlaneDistance = std::numeric_limits<float>::max()) const;

	Math::Ray MakeMouseRay(int x, int y) const;

private:
	boost::optional<Intersection> ShootRay(Math::Ray const &ray) const;

	std::vector<Observer<Light const>> LightsAtIntersection(
		Intersection const &intersection) const;

	Math::Ray ReflectRay(
		Math::Ray const &ray,
		Math::Vector3 const &origin,
		Math::Vector3 const &normal) const;

private:
	Scene const &scene;
	RayTracerConfiguration configuration;
};

static_assert(std::is_copy_constructible_v<RayTracer>);
static_assert(!std::is_copy_assignable_v<RayTracer>);
#ifdef _MSC_VER
static_assert(!std::is_trivially_copyable_v<RayTracer>);
#else
static_assert(std::is_trivially_copyable_v<RayTracer>);
#endif

static_assert(std::is_move_constructible_v<RayTracer>);
static_assert(!std::is_move_assignable_v<RayTracer>);
} // namespace LibRay

#endif // ef875083_56da_287e_58f0_a7a130757a7d
