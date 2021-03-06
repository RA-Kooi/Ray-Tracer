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

namespace Materials
{
class Color;
} // namespace Materials

class Image;
class Intersection;
class Light;
class Scene;

struct LIBRAY_API RayTracerConfiguration final
{
	enum class AntiAliasingMode: size_t
	{
		AA1  =  1,
		AA2  =  2,
		AA4  =  4,
		AA8  =  8,
		AA16 = 16
	};

	RayTracerConfiguration(
		std::uint8_t maxReflectionBounces,
		std::uint8_t threadCount,
		AntiAliasingMode aaMode);

	std::uint8_t maxReflectionBounces;
	std::uint8_t threadCount;
	AntiAliasingMode aaMode;
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
	static_assert(std::is_trivially_copyable_v<RayState>);

	static_assert(std::is_move_constructible_v<RayState>);
	static_assert(std::is_move_assignable_v<RayState>);

public:
	RayTracer(Scene const &scene, RayTracerConfiguration &&config);

	Image Trace() const;
	Materials::Color TraceRay(
		Math::Ray const &ray,
		RayState &state,
		bool debug = false,
		float farPlaneDistance = std::numeric_limits<float>::max()) const;

	Math::Ray MakeMouseRay(int x, int y) const;

private:
	void TraceChunk(
		Image &output,
		std::size_t chunkLength,
		std::size_t chunkWidth,
		std::size_t chunkStartX,
		std::size_t chunkStartY,
		float worldFarDistance) const;

	std::optional<Intersection> ShootRay(Math::Ray const &ray) const;

	std::vector<Observer<Light const>> LightsAtIntersection(
		Intersection const &intersection) const;

	Materials::Color DoReflection(
		Intersection const &intersection,
		Math::Ray const &ray,
		RayState &state,
		bool debug,
		float farPlaneDistance) const;

	Materials::Color DoRefraction(
		Intersection const &intersection,
		Math::Ray const &ray,
		RayState &state,
		bool debug,
		float farPlaneDistance) const;

	Materials::Color Shade(
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

	float FresnelFactor(float cosTheta, float iorA, float iorB) const;

private:
	Scene const &scene;
	RayTracerConfiguration configuration;
	std::vector<Math::Vector2> raySteps;
};

static_assert(std::is_copy_constructible_v<RayTracer>);
static_assert(!std::is_copy_assignable_v<RayTracer>);
static_assert(!std::is_trivially_copyable_v<RayTracer>);

static_assert(std::is_move_constructible_v<RayTracer>);
static_assert(!std::is_move_assignable_v<RayTracer>);
} // namespace LibRay

#endif // ef875083_56da_287e_58f0_a7a130757a7d
