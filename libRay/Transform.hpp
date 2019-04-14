#ifndef ea59b955_a132_c215_6059_43e1feb21f2e
#define ea59b955_a132_c215_6059_43e1feb21f2e

#include <type_traits>

#include "Math/Matrix.hpp"
#include "Math/Quaternion.hpp"
#include "Math/Vector.hpp"
#include "API.hpp"

namespace LibRay
{
class LIBRAY_API Transform final
{
public:
	Transform();

	explicit Transform(
		Math::Vector3 const &position,
		Math::Quaternion const &rotation = Math::Quaternion::Identity(),
		Math::Vector3 const &scale = Math::Vector3::Ones());

	Transform(
		Math::Vector3 const &position,
		Eigen::AngleAxis<float> const &rotation,
		Math::Vector3 const &scale = Math::Vector3::Ones());

	void Translate(Math::Vector3 const &translation);
	void Move(Math::Vector3 const &newPosition);
	Math::Vector3 const &Position() const;

	void AddRotation(Math::Quaternion const &extraRotation);
	void Rotate(Math::Quaternion const &newRotation);
	Math::Quaternion const &Rotation() const;

	void Scale(Math::Vector3 const &extraScale);
	void Rescale(Math::Vector3 const &newScale);
	Math::Vector3 const &Scale() const;

	Math::Matrix4x4 const &Matrix() const;
	Math::Matrix4x4 const &InverseMatrix() const;

	static Math::Vector3 TransformDirection(
		Math::Matrix4x4 const &matrix,
		Math::Vector3 const &direction);

	static Math::Vector3 TransformTranslation(
		Math::Matrix4x4 const &matrix,
		Math::Vector3 const &translation);

	void RecalculateMatrix();
	bool IsDirty() const;

private:
	Math::Vector3 position, scale;
	Math::Quaternion rotation;

	Math::Matrix4x4 matrix, inverseMatrix;
	bool dirty;
};

static_assert(std::is_copy_constructible_v<Transform>);
static_assert(std::is_copy_assignable_v<Transform>);
static_assert(!std::is_trivially_copyable_v<Transform>);

static_assert(std::is_move_constructible_v<Transform>);
static_assert(std::is_move_assignable_v<Transform>);
} // namespace LibRay

#endif // ea59b955_a132_c215_6059_43e1feb21f2e
