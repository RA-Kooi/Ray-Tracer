#include "RayTracer.hpp"

#include <algorithm>
#include <iostream>
#include <optional>

#include "Material/Color.hpp"
#include "Material/Material.hpp"
#include "Math/MathUtils.hpp"
#include "Math/Matrix.hpp"
#include "Math/Ray.hpp"
#include "Shaders/Shader.hpp"
#include "Shapes/Shape.hpp"
#include "Threading/TaskProcessor.hpp"
#include "Image.hpp"
#include "Intersection.hpp"
#include "Light.hpp"
#include "Scene.hpp"
#include "Utilites.hpp"

namespace LibRay
{
using namespace Materials;
using namespace Math;
using namespace Shapes;

constexpr float const bias = 0.005f;

static std::string indent(int n)
{
	return std::string(size_t(n), '\t');
}

RayTracerConfiguration::RayTracerConfiguration(
	std::uint8_t maxReflectionBounces,
	std::uint8_t threadCount,
	AntiAliasingMode aaMode)
: maxReflectionBounces(maxReflectionBounces)
, threadCount(threadCount)
, aaMode(aaMode)
{
}

RayTracer::RayTracer(Scene const &scene, RayTracerConfiguration &&config)
: scene(scene)
, configuration(std::move(config))
{
	raySteps.reserve(size_t(configuration.aaMode));

	Camera const &camera = scene.Camera();
	Vector2st const &screenSize = camera.ScreenSize();
	Camera::Frustum const frustum = camera.SceneFrustum();

	float const nearPlaneWidth = frustum.nearPlaneHeight * frustum.aspectRatio;

	float const fullStepX = nearPlaneWidth / float(screenSize.x);
	float const fullStepy = frustum.nearPlaneHeight / float(screenSize.y);

	switch(configuration.aaMode)
	{
		case RayTracerConfiguration::AntiAliasingMode::AA1:
		{
			raySteps.emplace_back(0.5f * fullStepX, 0.5f * fullStepy);
		} break;
		case RayTracerConfiguration::AntiAliasingMode::AA2:
		{
			raySteps.emplace_back(0.25f * fullStepX, 0.5f * fullStepy);
			raySteps.emplace_back(0.75f * fullStepX, 0.5f * fullStepy);
		} break;
		case RayTracerConfiguration::AntiAliasingMode::AA4:
		{
			raySteps.emplace_back(0.25f * fullStepX, 0.25f * fullStepy);
			raySteps.emplace_back(0.75f * fullStepX, 0.75f * fullStepy);

			raySteps.emplace_back(0.25f * fullStepX, 0.25f * fullStepy);
			raySteps.emplace_back(0.75f * fullStepX, 0.75f * fullStepy);
		} break;
		case RayTracerConfiguration::AntiAliasingMode::AA8:
		{
			raySteps.emplace_back(0.2f * fullStepX, 0.25f * fullStepy);
			raySteps.emplace_back(0.4f * fullStepX, 0.25f * fullStepy);
			raySteps.emplace_back(0.6f * fullStepX, 0.25f * fullStepy);
			raySteps.emplace_back(0.8f * fullStepX, 0.25f * fullStepy);

			raySteps.emplace_back(0.2f * fullStepX, 0.75f * fullStepy);
			raySteps.emplace_back(0.4f * fullStepX, 0.75f * fullStepy);
			raySteps.emplace_back(0.6f * fullStepX, 0.75f * fullStepy);
			raySteps.emplace_back(0.8f * fullStepX, 0.75f * fullStepy);
		} break;
		case RayTracerConfiguration::AntiAliasingMode::AA16:
		{
			raySteps.emplace_back(0.2f * fullStepX, 0.2f * fullStepy);
			raySteps.emplace_back(0.4f * fullStepX, 0.2f * fullStepy);
			raySteps.emplace_back(0.6f * fullStepX, 0.2f * fullStepy);
			raySteps.emplace_back(0.8f * fullStepX, 0.2f * fullStepy);

			raySteps.emplace_back(0.2f * fullStepX, 0.4f * fullStepy);
			raySteps.emplace_back(0.4f * fullStepX, 0.4f * fullStepy);
			raySteps.emplace_back(0.6f * fullStepX, 0.4f * fullStepy);
			raySteps.emplace_back(0.8f * fullStepX, 0.4f * fullStepy);

			raySteps.emplace_back(0.2f * fullStepX, 0.6f * fullStepy);
			raySteps.emplace_back(0.4f * fullStepX, 0.6f * fullStepy);
			raySteps.emplace_back(0.6f * fullStepX, 0.6f * fullStepy);
			raySteps.emplace_back(0.8f * fullStepX, 0.6f * fullStepy);

			raySteps.emplace_back(0.2f * fullStepX, 0.8f * fullStepy);
			raySteps.emplace_back(0.4f * fullStepX, 0.8f * fullStepy);
			raySteps.emplace_back(0.6f * fullStepX, 0.8f * fullStepy);
			raySteps.emplace_back(0.8f * fullStepX, 0.8f * fullStepy);
		} break;
	}
}

Image RayTracer::Trace() const
{
	Stopwatch watch;
	watch.Start();

	Camera const &camera = scene.Camera();
	Vector2st const &screenSize = camera.ScreenSize();
	Camera::Frustum const frustum = camera.SceneFrustum();

	Image output(std::size_t(screenSize.x), std::size_t(screenSize.y));
	output.pixels.resize(std::size_t(screenSize.x * screenSize.y), Color::Black());

	Matrix4x4 const camToWorld = camera.Transform().Matrix();

	Vector3 const worldFar = Transform::TransformTranslation(
		camToWorld,
		Vector3(0, 0, -1) * frustum.farPlaneDistance);

	Vector3 const cameraPosition = camera.Transform().Position();

	float const worldFarDistance = glm::length2(worldFar - cameraPosition);

	Threading::TaskProcessor scheduler(configuration.threadCount);

	using ssize_t = std::make_signed_t<size_t>;
	std::lldiv_t const xSplit = std::lldiv(ssize_t(screenSize.x), 64);

	std::lldiv_t const ySplit = std::lldiv(ssize_t(screenSize.y), 64);

	std::size_t const xCount = size_t(xSplit.quot);
	std::size_t const xOffWidth = size_t(64 + xSplit.rem);

	std::size_t const yCount = size_t(ySplit.quot);
	std::size_t const yOffWidth = size_t(64 + ySplit.rem);

	for(size_t i = 0; i < yCount; ++i)
	{
		for(size_t j = 0; j < xCount; ++j)
		{
			scheduler.AddTask(
				[
					this,
					&output,
					worldFarDistance,
					xCount,
					yCount,
					i,
					j,
					xOffWidth,
					yOffWidth
				]()
				{
					RayTracer::TraceChunk(
						output,
						i < yCount - 1 ? 64 : yOffWidth,
						j < xCount - 1 ? 64 : xOffWidth,
						64 * j,
						64 * i,
						worldFarDistance);
				});
		}
	}

	scheduler.Run();

	watch.Stop();

	std::cout << "Took " << watch.Value() << " to render the scene\n";
	std::fflush(stdout);

	return output;
}

void RayTracer::TraceChunk(
	Image &output,
	std::size_t chunkLength,
	std::size_t chunkWidth,
	std::size_t chunkStartX,
	std::size_t chunkStartY,
	float worldFarDistance) const
{
	Camera const &camera = scene.Camera();
	Vector2st const &screenSize = camera.ScreenSize();
	Camera::Frustum const frustum = camera.SceneFrustum();

	float const nearPlaneWidth = frustum.nearPlaneHeight * frustum.aspectRatio;
	float const halfNearPlaneWidth = 0.5f * nearPlaneWidth;
	float const halfNearPlaneHeight = 0.5f * frustum.nearPlaneHeight;

	Vector3 const cameraPosition = camera.Transform().Position();

	Vector3 const nearPlanePosition = Vector3(0, 0, -1)
		* frustum.nearPlaneDistance;

	Vector3 const nearPlaneTopLeft = nearPlanePosition
		- Vector3(halfNearPlaneWidth, -halfNearPlaneHeight, 0);

	float const stepX = nearPlaneWidth / float(screenSize.x);
	float const stepY = frustum.nearPlaneHeight / float(screenSize.y);

	Matrix4x4 const camToWorld = camera.Transform().Matrix();

	for(std::size_t y = chunkStartY; y < chunkStartY + chunkLength; ++y)
	{
		for(std::size_t x = chunkStartX; x < chunkStartX + chunkWidth; ++x)
		{
			Color pixel = Color::Black();
			size_t const sampleCount = size_t(configuration.aaMode);

			for(size_t i = 0; i < sampleCount; ++i)
			{
				Vector2 const &steps = raySteps[i];

				float const u = nearPlaneTopLeft.x + x * stepX + steps.x;
				float const v = nearPlaneTopLeft.y - y * stepY - steps.y;

				Vector3 rayTarget(u, v, -1);
				rayTarget = glm::normalize(rayTarget);

				Ray const ray(
					cameraPosition + rayTarget * frustum.nearPlaneDistance,
					Transform::TransformDirection(camToWorld, rayTarget));

				RayState state;
				pixel += TraceRay(ray, state, false, worldFarDistance);
			}

			pixel /= float(sampleCount);

			output.pixels[y * screenSize.x + x] = pixel;
		}
	}
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
			std::cout << indent(state.bounceCount) << "No intersections...\n\n";

		return Color::Black();
	}

	if(debug)
		std::cout << indent(state.bounceCount) << "{\n";

	Vector3 const &intersectionPos = intersection->worldPosition;
	Vector3 const &normal = intersection->surfaceNormal;

	if(debug)
	{
		std::cout << indent(state.bounceCount + 1)
			<< "Intersection:\n"
			<< indent(state.bounceCount + 1)
			<< "{\n"
			<< indent(state.bounceCount + 2)
			<< "Pos: " << intersectionPos << ",\n"
			<< indent(state.bounceCount + 2)
			<< "Normal: " << normal << "\n"
			<< indent(state.bounceCount + 1)
			<< "},\n\n";
	}

	float const intersectionDistance =
		glm::length2(scene.Camera().Transform().Position() - intersectionPos);

	if(intersectionDistance > farPlaneDistance)
	{
		if(debug)
		{
			std::cout << indent(state.bounceCount + 1)
				<< "No closest intersection picked..."
				<< indent(state.bounceCount + 1)
				<< "\n}\n";
		}

		return Color::Black();
	}

	if(debug)
	{
		Vector3 const &pos = intersection->worldPosition;
		Vector2 const &uv = intersection->uv;

		std::cout << indent(state.bounceCount + 1)
			<< "Picked intersection:\n"
			<< indent(state.bounceCount + 1)
			<<"{\n"
			<< indent(state.bounceCount + 2)
			<< "Pos: " << pos << ",\n"
			<< indent(state.bounceCount + 2)
			<< "Normal: " << normal << ",\n"
			<< indent(state.bounceCount + 2)
			<< "UV: " << uv << "\n"
			<< indent(state.bounceCount + 1)
			<< "},\n\n";
	}

	Material const &material = intersection->shape->Material();

	float const indexOfRefraction = material.RefractiveIndexInside();
	if(indexOfRefraction > 0.f
	   && state.bounceCount < configuration.maxReflectionBounces)
	{
		return DoRefraction(*intersection, ray, state, debug, farPlaneDistance);
	}

	float const reflectiveness = material.Reflectiveness();
	if(reflectiveness > 0.f
	   && indexOfRefraction <= 0.f
	   && state.bounceCount < configuration.maxReflectionBounces)
	{
		return DoReflection(*intersection, ray, state, debug, farPlaneDistance);
	}

	Color const pixelColor = Shade(ray, *intersection, debug);

	if(debug)
	{
		std::cout << indent(state.bounceCount + 1)
			<< "Final pixel color: " << pixelColor << "\n"
			<< indent(state.bounceCount)
			<< "}\n\n";
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
		std::cout << indent(state.bounceCount + 1)
			<< "Reflecting... (no refraction)\n\n";

	Vector3 const &intersectionPos = intersection.worldPosition;
	Vector3 const &normal = intersection.surfaceNormal;

	Vector3 N = normal;

	float const cosTheta = glm::dot(normal, ray.Direction());
	if(cosTheta > 0.f)
		N = -N;

	Ray const reflectedRay = ReflectRay(ray, intersectionPos, N);

	if(debug)
	{
		std::cout << indent(state.bounceCount + 1)
			<< "Ray:\n"
			<< indent(state.bounceCount + 1)
			<< "{\n"
			<< indent(state.bounceCount + 2)
			<< "Origin: " << ray.Origin() << ",\n"
			<< indent(state.bounceCount + 2)
			<< "Direction: " << ray.Direction() << "\n"
			<< indent(state.bounceCount + 1)
			<< "},\n\n";

		std::cout << indent(state.bounceCount + 1)
			<< "Reflection ray:\n"
			<< indent(state.bounceCount + 1)
			<< "{\n"
			<< indent(state.bounceCount + 2)
			<< "Origin: " << reflectedRay.Origin() << ",\n"
			<< indent(state.bounceCount + 2)
			<< "Direction: " << reflectedRay.Direction() << "\n"
			<< indent(state.bounceCount + 1)
			<< "},\n\n";
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

	if(debug)
	{
		std::cout << indent(state.bounceCount + 1)
			<< "Final pixel color: " << pixelColor << "\n"
			<< indent(state.bounceCount)
			<< "}\n\n";
	}

	return pixelColor;
}

Color RayTracer::DoRefraction(
	Intersection const &intersection,
	Ray const &ray,
	RayState &state,
	bool debug,
	float farPlaneDistance) const
{
	if(debug)
	{
		std::cout << indent(state.bounceCount + 1)
			<< "Refracting...\n\n";

		std::cout << indent(state.bounceCount + 1)
			<< "Ray:\n"
			<< indent(state.bounceCount + 1)
			<< "{\n"
			<< indent(state.bounceCount + 2)
			<< "Origin: " << ray.Origin() << ",\n"
			<< indent(state.bounceCount + 2)
			<< "Direction: " << ray.Direction() << "\n"
			<< indent(state.bounceCount + 1)
			<< "},\n\n";
	}

	Vector3 const &intersectionPos = intersection.worldPosition;
	Vector3 const &normal = intersection.surfaceNormal;

	Material const &material = intersection.shape->Material();
	float iorInside = material.RefractiveIndexInside();

	float iorOutside = material.RefractiveIndexOutside();

	Vector3 const &incidence = ray.Direction();
	float cosTheta = glm::dot(incidence, normal);
	float const cosThetaOrig = cosTheta;
	bool exiting = false;

	Vector3 N = normal;
	if(cosTheta < 0)
	{
		cosTheta = -cosTheta;
	}
	else
	{
		N = -N;
		std::swap(iorInside, iorOutside);
		exiting = true;
	}

	if(debug)
	{
		std::cout << indent(state.bounceCount + 1)
			<< "CosTheta: " << cosThetaOrig << ",\n";
	}

	float const fresnel = FresnelFactor(cosThetaOrig, iorOutside, iorInside);
	Color refractedColor = Color::Black();

	if(debug)
	{
		std::cout << indent(state.bounceCount + 1)
			<< "Fresnel: " << fresnel << ",\n\n";
	}

	if(fresnel < 1.0f)
	{
		float const refractionRatio = iorOutside / iorInside;

		Vector3 const refractedDir = Refract(incidence, N, refractionRatio);

		Ray const refractedRay =
			Ray(intersectionPos - N * bias, refractedDir);

		if(debug)
		{
			std::cout << indent(state.bounceCount + 1)
				<< "Refraction ray:\n"
				<< indent(state.bounceCount + 1)
				<< "{\n"
				<< indent(state.bounceCount + 2)
				<< "Origin: " << refractedRay.Origin() << ",\n"
				<< indent(state.bounceCount + 2)
				<< "Direction: " << refractedRay.Direction() << "\n"
				<< indent(state.bounceCount + 1)
				<< "},\n\n";
		}

		if(debug)
		{
			if(!exiting)
				std::cout << indent(state.bounceCount + 1) << "Entering...\n";
			else
				std::cout << indent(state.bounceCount + 1) << "Leaving...\n";
		}

		++state.bounceCount;
		refractedColor = TraceRay(refractedRay, state, debug, farPlaneDistance);
		--state.bounceCount;

		if(debug)
		{
			std::cout << indent(state.bounceCount + 1)
				<< "Refracted color: " << refractedColor << ",\n\n";
		}
	}

	if(debug)
	{
		std::cout << indent(state.bounceCount + 1)
			<< "Reflecting... (refraction)\n\n";
	}

	Ray const reflectedRay = ReflectRay(ray, intersectionPos, normal);

	if(debug)
	{
		std::cout << indent(state.bounceCount + 1)
			<< "Reflection ray:\n"
			<< indent(state.bounceCount + 1)
			<< "{\n"
			<< indent(state.bounceCount + 2)
			<< "Origin: " << reflectedRay.Origin() << ",\n"
			<< indent(state.bounceCount + 2)
			<< "Direction: " << reflectedRay.Direction() << "\n"
			<< indent(state.bounceCount + 1)
			<< "},\n\n";
	}

	++state.bounceCount;
	Color const reflectedColor =
		TraceRay(reflectedRay, state, debug, farPlaneDistance);
	--state.bounceCount;

	if(debug)
	{
		std::cout << indent(state.bounceCount + 1)
			<< "Reflected color: " << reflectedColor << ",\n";
	}

	Color const pixelColor = reflectedColor * fresnel
		+ refractedColor * (1.f - fresnel);

	if(debug)
	{
		std::cout << indent(state.bounceCount + 1)
			<< "Final pixel color: " << pixelColor << "\n"
			<< indent(state.bounceCount)
			<< "}\n\n";
	}

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

	std::pair<Color const &, float> ambientLight = scene.AmbientLight();

	Color const pixelColor = shader.Run(
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

Vector3 RayTracer::Refract(
	Vector3 const &incidence,
	Vector3 const &normal,
	float refractionRatio) const
{
	float const cosTheta = glm::dot(incidence, normal);

	float const criticalAngle = 1.0f - refractionRatio * refractionRatio
		* (1.0f - cosTheta * cosTheta);

	if(criticalAngle >= 0)
		return refractionRatio * incidence
			- (refractionRatio * cosTheta + std::sqrt(criticalAngle)) * normal;
	else
		return Vector3(0);
}

float RayTracer::FresnelFactor(
	float cosTheta,
	float iorA,
	float iorB) const
{
	float cosThetaI = Math::Clamp(cosTheta, -1.f, 1.f);

	float const sini = iorA / iorB
		* std::sqrt(std::max(0.f, 1.0f - cosThetaI * cosThetaI));

	if(sini >= 1.0f)
		return 1.0f;

	float const cost = std::sqrt(std::max(0.f, 1.0f - sini * sini));

	cosThetaI = std::fabs(cosThetaI);

	float const ACost = iorA * cost;
	float const BCost = iorB * cost;

	float const ATheta = iorA * cosThetaI;
	float const BTheta = iorB * cosThetaI;

	float const parallel = (BTheta - ACost) / (BTheta + ACost);
	float const perpendicular = (ATheta - BCost) / (ATheta + BCost);

	return (parallel * parallel + perpendicular * perpendicular) / 2;
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
	Containers::BVH<Shape> const &bvh = scene.BoundingVolumeHierarchy();

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

			Material const &material = lightIntersection->shape->Material();
			float const ior = material.RefractiveIndexInside();

			if(intersectionDistance > lightDistance)
				unobstructedLights.push_back(&light);
			else if(ior > 0.f)
					unobstructedLights.push_back(&light);
		}
	}

	return unobstructedLights;
}
} // namespace LibRay
