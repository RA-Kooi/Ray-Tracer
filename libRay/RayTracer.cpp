#include "RayTracer.hpp"

#include <algorithm>
#include <optional>

#include "Math/Matrix.hpp"
#include "Math/Ray.hpp"
#include "Shaders/Material.hpp"
#include "Shaders/Shader.hpp"
#include "Shapes/Color.hpp"
#include "Shapes/Shape.hpp"
#include "Image.hpp"
#include "Intersection.hpp"
#include "Light.hpp"
#include "Scene.hpp"
#include "Utilites.hpp"

namespace LibRay
{
using namespace Shapes;
using namespace Math;

constexpr float const bias = 0.005f;

RayTracerConfiguration::RayTracerConfiguration(
	std::uint8_t maxReflectionBounces)
: maxReflectionBounces(maxReflectionBounces)
{
}

RayTracer::RayTracer(Scene const &scene, RayTracerConfiguration &&config)
: scene(scene)
, configuration(std::move(config))
{
}

Image RayTracer::Trace() const
{
	Stopwatch watch;
	watch.Start();

	Camera const &camera = scene.Camera();
	Vector2 const &screenSize = camera.ScreenSize();
	Camera::Frustum const frustum = camera.SceneFrustum();

	Image output(std::size_t(screenSize.x), std::size_t(screenSize.y));

	float const nearPlaneWidth = frustum.nearPlaneHeight * frustum.aspectRatio;
	float const halfNearPlaneWidth = 0.5f * nearPlaneWidth;
	float const halfNearPlaneHeight = 0.5f * frustum.nearPlaneHeight;

	Vector3 const cameraPosition = camera.Transform().Position();

	Vector3 const nearPlanePosition = Vector3(0, 0, -1)
		* frustum.nearPlaneDistance;

	Vector3 const nearPlaneTopLeft = nearPlanePosition
		- Vector3(halfNearPlaneWidth, -halfNearPlaneHeight, 0);

	float const stepX = nearPlaneWidth / screenSize.x;
	float const halfStepX = 0.5f * stepX;
	float const stepY = frustum.nearPlaneHeight / screenSize.y;
	float const halfStepY = 0.5f * stepY;

	Matrix4x4 const camToWorld = camera.Transform().Matrix();

	Vector3 const worldFar = Transform::TransformTranslation(
		camToWorld,
		Vector3(0, 0, -1) * frustum.farPlaneDistance);

	float const worldFarDistance = glm::length2(worldFar - cameraPosition);

	for(int y = int(screenSize.y) - 1; y >= 0; --y)
	{
		for(int x = 0; x < int(screenSize.x); ++x)
		{
			float const u = nearPlaneTopLeft.x + x * stepX + halfStepX;
			float const v = nearPlaneTopLeft.y - y * stepY - halfStepY;

			Vector3 rayTarget(u, -v, -1);
			rayTarget = glm::normalize(rayTarget);

			Ray const ray(
				cameraPosition + rayTarget * frustum.nearPlaneDistance,
				Transform::TransformDirection(camToWorld, rayTarget));

			RayState state;
			output.pixels.push_back(
				TraceRay(ray, state, false, worldFarDistance));
		}
	}

	watch.Stop();

	std::printf("Took %s to render the scene\n", watch.Value().c_str());
	std::fflush(stdout);

	return output;
}

Color RayTracer::TraceRay(
	Ray const &ray,
	RayState &state,
	bool debug,
	float farPlaneDistance) const
{
	std::optional<Intersection> intersection = ShootRay(ray);

	if(!intersection)
	{
		if(debug)
			std::puts("No intersections...\n");

		return Color::Black();
	}

	if(debug)
		std::puts("{");

	Vector3 const &intersectionPos = intersection->worldPosition;

	if(debug)
	{
		std::printf(
			"\tIntersection:\n\t{\n"
			"\t\tPos: [X: %f, Y: %f, Z: %f],\n"
			"\t\tNormal: [X: %f, Y: %f, Z: %f]\n"
			"\t},\n\n",
			double(intersectionPos.x),
			double(intersectionPos.y),
			double(intersectionPos.z),
			double(intersection->surfaceNormal.x),
			double(intersection->surfaceNormal.y),
			double(intersection->surfaceNormal.z));
	}

	float const intersectionDistance =
		glm::length2(scene.Camera().Transform().Position() - intersectionPos);

	if(intersectionDistance > farPlaneDistance)
	{
		if(debug)
			std::puts("\tNo closest intersection picked...\n}\n");

		return Color::Black();
	}

	Vector3 const &normal = intersection->surfaceNormal;

	if(debug)
	{
		Vector3 const pos = intersection->worldPosition;

		std::printf(
			"\tPicked intersection:\n\t{\n"
			"\t\tPos: [X: %f, Y: %f, Z: %f],\n"
			"\t\tNormal: [X: %f, Y: %f, Z: %f]\n"
			"\t},\n\n",
			double(pos.x), double(pos.y), double(pos.z),
			double(normal.x), double(normal.y), double(normal.z));
	}

	Material const &material = intersection->shape->Material();

	float const reflectiveness = material.Reflectiveness();
	if(reflectiveness > 0.f
	   && state.bounceCount < configuration.maxReflectionBounces)
	{
		return DoReflection(*intersection, ray, state, debug, farPlaneDistance);
	}

	Color pixelColor = Shade(ray, *intersection, debug);

	if(debug)
	{
		std::printf(
			"\tFinal pixel color: %s\n",
			pixelColor.ToString().c_str());

		std::puts("}\n");
	}

	return pixelColor;
}

Color RayTracer::DoReflection(
	Intersection const &intersection,
	Ray const &ray,
	RayState &state,
	bool debug,
	float farPlaneDistance) const
{
	if(debug)
		std::puts("\n\tReflecting... (no refraction)");

	Vector3 const &intersectionPos = intersection.worldPosition;
	Vector3 const &normal = intersection.surfaceNormal;

	Vector3 N = normal;

	float const cosTheta = glm::dot(normal, ray.Direction());
	if(cosTheta > 0.f)
		N = -N;

	Ray const reflectedRay = ReflectRay(ray, intersectionPos, N);

	if(debug)
	{
		std::printf("\tRay:\n\t{\n\t\t%s\n\t},\n\n", ray.ToString().c_str());

		std::printf(
			"\tReflection ray:\n\t{\n\t\t%s\n\t},\n\n",
			reflectedRay.ToString().c_str());
	}

	++state.bounceCount;
	Color const reflectedColor =
		TraceRay(reflectedRay, state, debug, farPlaneDistance);
	--state.bounceCount;

	Material const &material = intersection.shape->Material();
	float const reflectiveness = material.Reflectiveness();

	Color pixelColor = Shade(ray, intersection, debug);
	pixelColor *= (1.f - reflectiveness);
	pixelColor += reflectedColor * reflectiveness;

	return pixelColor;
}

Color RayTracer::Shade(
	Ray const &ray,
	Intersection const &intersection,
	bool debug) const
{
	Vector3 const &cameraPosition = scene.Camera().Transform().Position();
	Vector3 const &intersectionPos = intersection.worldPosition;
	Vector3 const view = glm::normalize(cameraPosition - intersectionPos);

	std::vector<Observer<Light const>> unobstructedLights =
		LightsAtIntersection(intersection);

	if(debug)
		std::printf("\tLight count: %zu,\n", unobstructedLights.size());

	Material const &material = intersection.shape->Material();
	Shader const &shader = material.Shader();

	std::pair<Shapes::Color const &, float> ambientLight = scene.AmbientLight();

	Color pixelColor = shader.Run(
			intersection,
			view,
			ray,
			unobstructedLights,
			ambientLight.first,
			ambientLight.second);

	return pixelColor;
}

Ray RayTracer::ReflectRay(
	Ray const &ray,
	Vector3 const &origin,
	Vector3 const &normal) const
{
	Vector3 const view = -ray.Direction();
	float const cosTheta = glm::dot(view, normal);
	Vector3 const viewOnNormalProjection = cosTheta * normal;
	Vector3 const reflected = view + 2.f * (viewOnNormalProjection - view);

	return Ray(origin + normal * bias, reflected);
}

Ray RayTracer::MakeMouseRay(int x, int y) const
{
	Camera const &camera = scene.Camera();
	Vector2 const &screenSize = camera.ScreenSize();
	Camera::Frustum const frustum = camera.SceneFrustum();

	float const nearPlaneWidth = frustum.nearPlaneHeight * frustum.aspectRatio;
	float const halfNearPlaneWidth = 0.5f * nearPlaneWidth;
	float const halfNearPlaneHeight = 0.5f * frustum.nearPlaneHeight;

	Vector3 const cameraPosition = camera.Transform().Position();
	Vector3 const nearPlanePosition = Vector3(0, 0, -1)
		* frustum.nearPlaneDistance;
	Vector3 const nearPlaneTopLeft = nearPlanePosition
		- Vector3(halfNearPlaneWidth, -halfNearPlaneHeight, 0);

	float const stepX = nearPlaneWidth / screenSize.x;
	float const halfStepX = 0.5f * stepX;
	float const stepY = frustum.nearPlaneHeight / screenSize.y;
	float const halfStepY = 0.5f * stepY;

	float const u = nearPlaneTopLeft.x + x * stepX + halfStepX;
	float const v = nearPlaneTopLeft.y - y * stepY - halfStepY;

	Vector3 const rayTarget(u, v, -1);

	Matrix4x4 const camToWorld = camera.Transform().Matrix();

	Ray const ray(
		cameraPosition,
		Transform::TransformDirection(camToWorld, rayTarget));

	return ray;
}

std::optional<Intersection> RayTracer::ShootRay(Ray const &ray) const
{
	Containers::BVH const &bvh = scene.BoundingVolumeHierarchy();

	std::optional<Intersection> closestIntersection = bvh.Traverse(ray);
	float closestDistance = FLT_MAX;
	if(closestIntersection)
	{
		Vector3 const intersectionToOrigin =
			(closestIntersection->worldPosition - ray.Origin());
		closestDistance = glm::length2(intersectionToOrigin);
	}

	for(auto const &shape: scene.UnboundableShapes())
	{
		std::optional<Intersection> const intersection = shape->Intersects(ray);
		if(intersection)
		{
			Vector3 const intersectionToOrigin =
				(intersection->worldPosition - ray.Origin());
			float const distance = glm::length2(intersectionToOrigin);

			if(closestIntersection)
			{
				if(distance < closestDistance)
				{
					closestIntersection = intersection;
					closestDistance = distance;
				}
			}
			else
			{
				closestIntersection = intersection;
				closestDistance = distance;
			}
		}
	}

	return closestIntersection;
}

std::vector<Observer<Light const>> RayTracer::LightsAtIntersection(
	Intersection const &intersection) const
{
	std::vector<Light> const &lights = scene.Lights();
	std::vector<Observer<Light const>> unobstructedLights;

	for(Light const &light: lights)
	{
		Vector3 const biasedOrigin = intersection.worldPosition
			+ intersection.surfaceNormal
			* bias;

		Ray const lightRay(
			biasedOrigin,
			light.Position() - intersection.worldPosition);

		float const lightDistance =
			glm::length2(light.Position() - biasedOrigin);

		std::optional<Intersection> lightIntersection = ShootRay(lightRay);

		if(!lightIntersection)
			unobstructedLights.push_back(&light);
		else
		{
			Vector3 const intersectionToOrigin =
				lightIntersection->worldPosition - biasedOrigin;

			float const intersectionDistance = glm::length2(intersectionToOrigin);

			if(intersectionDistance > lightDistance)
				unobstructedLights.push_back(&light);
		}
	}

	return unobstructedLights;
}
} // namespace LibRay
