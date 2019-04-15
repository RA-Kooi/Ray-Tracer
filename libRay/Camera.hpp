#ifndef ab362926_1743_ba54_51a4_267f08a6b5ae
#define ab362926_1743_ba54_51a4_267f08a6b5ae

#include <type_traits>

#include "Math/Vector.hpp"
#include "API.hpp"
#include "Transform.hpp"

namespace LibRay
{
class LIBRAY_API Camera final
{
public:
	class LIBRAY_API Frustum final
	{
	public:
		Frustum(
			float fovY,
			float aspectRatio,
			float nearPlaneHeight,
			float nearPlaneDistance,
			float farPlaneHeight,
			float farPlaneDistance);

		float fovY, aspectRatio;
		float nearPlaneHeight, nearPlaneDistance;
		float farPlaneHeight, farPlaneDistance;
	};

	static_assert(std::is_copy_constructible_v<Frustum>);
	static_assert(std::is_copy_assignable_v<Frustum>);
	static_assert(std::is_trivially_copyable_v<Frustum>);

	static_assert(std::is_move_constructible_v<Frustum>);
	static_assert(std::is_move_assignable_v<Frustum>);

public:
	Camera(
		Transform const &transform,
		Math::Vector2 const &screenSize,
		float fovY,
		float near,
		float far);

	Frustum SceneFrustum() const;
	Math::Vector2 const &ScreenSize() const;

	Transform const &Transform() const;

private:
	class Transform transform;
	Math::Vector2 screenSize;

	float fovY, nearPlane, farPlane;
};

static_assert(std::is_copy_constructible_v<Camera>);
static_assert(std::is_copy_assignable_v<Camera>);
static_assert(std::is_trivially_copyable_v<Camera>);

static_assert(std::is_move_constructible_v<Camera>);
static_assert(std::is_move_assignable_v<Camera>);
} // namespace LibRay

#endif // ab362926_1743_ba54_51a4_267f08a6b5ae
