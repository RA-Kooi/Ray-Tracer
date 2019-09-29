#ifndef ef875083_56da_287e_58f0_a7a130757a7d
#define ef875083_56da_287e_58f0_a7a130757a7d

#include <limits>
#include <optional>
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
	class RayState
	{
	public:
		RayState() = default;

	public:
		std::uint8_t bounceCount = 0;
	};

	static_assert(std::is_copy_constructible_v<RayState>);
	static_assert(std::is_copy_assignable_v<RayState>);
	static_assert(!std::is_trivially_copyable_v<RayState>);

	static_assert(std::is_move_constructible_v<RayState>);
	static_assert(std::is_move_assignable_v<RayState>);

public:
	RayTracer(Scene const &scene, RayTracerConfiguration &&config);

	Image Trace() const;
	Shapes::Color TraceRay(
		Math::Ray const &ray,
		RayState &state,
		bool debug = false,
		float farPlaneDistance = std::numeric_limits<float>::max()) const;

	Math::Ray MakeMouseRay(int x, int y) const;

private:
	std::optional<Intersection> ShootRay(Math::Ray const &ray) const;

	std::vector<Observer<Light const>> LightsAtIntersection(
		Intersection const &intersection) const;

	Shapes::Color DoReflection(
		Intersection const &intersection,
		Math::Ray const &ray,
		RayState &state,
		bool debug,
		float farPlaneDistance) const;

	Shapes::Color Shade(
		Math::Ray const &ray,
		Intersection const &intersection,
		bool debug) const;

	Math::Ray ReflectRay(
		Math::Ray const &ray,
		Math::Vector3 const &origin,
		Math::Vector3 const &normal) const;

	Math::Vector3 Refract(
		Math::Vector3 const &incidence,
		Math::Vector3 const &normal,
		float refractionRatio) const;

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
