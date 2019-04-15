#include "Camera.hpp"

#include <cmath>

namespace LibRay
{
using namespace Math;

Camera::Frustum::Frustum(
	float fovY,
	float aspectRatio,
	float nearPlaneHeight,
	float nearPlaneDistance,
	float farPlaneHeight,
	float farPlaneDistance)
: fovY(fovY)
, aspectRatio(aspectRatio)
, nearPlaneHeight(nearPlaneHeight)
, nearPlaneDistance(nearPlaneDistance)
, farPlaneHeight(farPlaneHeight)
, farPlaneDistance(farPlaneDistance)
{
}

Camera::Camera(
	class Transform const &transform,
	Vector2 const &screenSize,
	float fovY,
	float near,
	float far)
: transform(transform)
, screenSize(screenSize)
, fovY(fovY)
, nearPlane(near)
, farPlane(far)
{
}

Camera::Frustum Camera::SceneFrustum() const
{
	float const near = std::tan(fovY * 0.5f) * 2.f;
	float const nearPlaneHeight = near * nearPlane;
	float const farPlaneHeight = near * farPlane;

	return Frustum(
		fovY,
		screenSize.x / screenSize.y,
		nearPlaneHeight,
		nearPlane,
		farPlaneHeight,
		farPlane);
}

Vector2 const &Camera::ScreenSize() const
{
	return screenSize;
}

Transform const &Camera::Transform() const
{
	return transform;
}
} // namespace LibRay
