#ifndef ea59b955_a132_c215_6059_43e1feb21f2e
#define ea59b955_a132_c215_6059_43e1feb21f2e

#include <type_traits>

#include "Math/Matrix.hpp"
#include "Math/Vector.hpp"
#include "API.hpp"

namespace LibRay
{
class LIBRAY_API Transform final
{
public:
	explicit Transform(
		Math::Vector3 const &position,
		Math::Vector3 const &rotation = Math::Vector3(0),
		Math::Vector3 const &scale = Math::Vector3(1, 1, 1));

	void Translate(Math::Vector3 const &translation);
	void Move(Math::Vector3 const &newPosition);
	Math::Vector3 const &Position() const;

	void AddRotation(Math::Vector3 const &eulerAngles);
	void Rotate(Math::Vector3 const &eulerAngles);
	Math::Vector3 const &Rotation() const;

	void Scale(Math::Vector3 const &extraScale);
	void Rescale(Math::Vector3 const &newScale);
	Math::Vector3 const &Scale() const;

	Math::Matrix4x4 const &Matrix() const;
	Math::Matrix4x4 const &InverseMatrix() const;

	void RecalculateMatrix();

	static Math::Vector3 TransformDirection(
		Math::Matrix4x4 const &matrix,
		Math::Vector3 const &direction);

	static Math::Vector3 TransformTranslation(
		Math::Matrix4x4 const &matrix,
		Math::Vector3 const &translation);

private:
	Math::Vector3 position, rotation, scale;

	Math::Matrix4x4 matrix, inverseMatrix;
	bool dirty = true;
};

static_assert(std::is_copy_constructible_v<Transform>);
static_assert(std::is_copy_assignable_v<Transform>);
static_assert(std::is_trivially_copyable_v<Transform>);

static_assert(std::is_move_constructible_v<Transform>);
static_assert(std::is_move_assignable_v<Transform>);
} // namespace LibRay

#endif // ea59b955_a132_c215_6059_43e1feb21f2e
