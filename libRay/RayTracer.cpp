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
				Transform::TransformTranslation(
					camToWorld,
					cameraPosition + rayTarget * frustum.nearPlaneDistance),
				Transform::TransformDirection(camToWorld, rayTarget));

			output.pixels.push_back(
				TraceRay(ray, 0, false, worldFarDistance));
		}
	}

	watch.Stop();

	std::printf("Took %s to render the scene\n", watch.Value().c_str());
	std::fflush(stdout);

	return output;
}

Color RayTracer::TraceRay(
	Ray const &ray,
	std::uint8_t reflectionBounceCount,
	bool debug,
	float farPlaneDistance) const
{
	std::pair<Shapes::Color const &, float> ambientLight = scene.AmbientLight();

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
		glm::length2(ray.Origin() - intersectionPos);

	if(intersectionDistance > farPlaneDistance)
	{
		if(debug)
			std::puts("\tNo closest intersection picked...\n}\n");

		return Color::Black();
	}

	if(debug)
	{
		Vector3 const pos = intersection->worldPosition;
		Vector3 const normal = intersection->surfaceNormal;

		std::printf(
			"\tPicked intersection:\n\t{\n"
			"\t\tPos: [X: %f, Y: %f, Z: %f],\n"
			"\t\tNormal: [X: %f, Y: %f, Z: %f]\n"
			"\t},\n\n",
			double(pos.x), double(pos.y), double(pos.z),
			double(normal.x), double(normal.y), double(normal.z));
	}

	Vector3 const &cameraPosition = scene.Camera().Transform().Position();
	Vector3 const &normal = intersection->surfaceNormal;
	Vector3 const view = glm::normalize(cameraPosition - intersectionPos);

	std::vector<Observer<Light const>> unobstructedLights =
		LightsAtIntersection(*intersection);

	if(debug)
		std::printf("\tLight count: %zu,\n", unobstructedLights.size());

	Material const &material = intersection->shape->Material();
	Shader const &shader = material.Shader();
	Color pixelColor = shader.Run(
		*intersection,
		view,
		ray,
		unobstructedLights,
		ambientLight.first,
		ambientLight.second);

	if(debug)
	{
		std::printf(
			"\tPixel color: [r: %f, g: %f, b: %f]\n",
			double(pixelColor.r),
			double(pixelColor.g),
			double(pixelColor.b));
	}

	float const reflectiveness = material.Reflectiveness();
	if(reflectiveness > 0.f
	   && reflectionBounceCount < configuration.maxReflectionBounces)
	{
		if(debug)
			std::puts("\n\tReflecting...");

		Ray const reflectedRay = ReflectRay(ray, intersectionPos, normal);

		if(debug)
		{
			std::printf(
				"\tRay:\n\t{\n"
				"\t\tOrigin: [x: %f, y: %f, z: %f],\n"
				"\t\tDirection: [x: %f, y: %f, z: %f]\n"
				"\t},\n\n",
				double(ray.Origin().x),
				double(ray.Origin().y),
				double(ray.Origin().z),
				double(ray.Direction().x),
				double(ray.Direction().y),
				double(ray.Direction().z));

			std::printf(
				"\tReflection ray:\n\t{\n"
				"\t\tOrigin: [x: %f, y: %f, z: %f],\n"
				"\t\tDirection: [x: %f, y: %f, z: %f]\n"
				"\t},\n\n",
				double(reflectedRay.Origin().x),
				double(reflectedRay.Origin().y),
				double(reflectedRay.Origin().z),
				double(reflectedRay.Direction().x),
				double(reflectedRay.Direction().y),
				double(reflectedRay.Direction().z));
		}

		Color const reflectedColor = TraceRay(
			reflectedRay,
			++reflectionBounceCount,
			debug);

		pixelColor *= (1.f - reflectiveness);
		pixelColor += reflectedColor * reflectiveness;

		if(debug)
		{
			std::printf(
				"\tFinal pixel color: [r: %f, g: %f, b: %f]\n",
				double(pixelColor.r),
				double(pixelColor.g),
				double(pixelColor.b));
		}
	}

	if(debug)
		std::puts("}\n");

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
		Transform::TransformTranslation(camToWorld, cameraPosition),
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
