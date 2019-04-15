#include "Transform.hpp"

#include "Math/Quaternion.hpp"

namespace LibRay
{
using namespace Math;

Transform::Transform(
	Vector3 const &position,
	Vector3 const &rotation,
	Vector3 const &scale)
: position(position)
, rotation(rotation)
, scale(scale)
, matrix(Matrix4x4(1))
{
}

void Transform::Translate(Vector3 const &translation)
{
	dirty = true;

	position += translation;
}

void Transform::Move(Vector3 const &newPosition)
{
	dirty = true;

	position = newPosition;
}

Vector3 const &Transform::Position() const
{
	return position;
}

void Transform::AddRotation(Math::Vector3 const &eulerAngles)
{
	dirty = true;

	rotation += eulerAngles;
}

void Transform::Rotate(Math::Vector3 const &eulerAngles)
{
	dirty = true;

	rotation = eulerAngles;
}

Vector3 const &Transform::Rotation() const
{
	return rotation;
}

void Transform::Scale(Vector3 const &extraScale)
{
	dirty = true;

	scale += extraScale;
}

void Transform::Rescale(Vector3 const &newScale)
{
	dirty = true;

	scale = newScale;
}

Vector3 const &Transform::Scale() const
{
	return scale;
}

Matrix4x4 const &Transform::Matrix() const
{
	if(dirty)
		RecalculateMatrix();

	return matrix;
}

Matrix4x4 const &Transform::InverseMatrix() const
{
	if(dirty)
		RecalculateMatrix();

	return inverseMatrix;
}

Vector3 Transform::TransformDirection(
	Matrix4x4 const &matrix,
	Vector3 const &direction)
{
	return Matrix3x3(matrix) * direction;
}

Vector3 Transform::TransformTranslation(
	Matrix4x4 const &matrix,
	Vector3 const &translation)
{
	Vector4 const transformedTranslation = (matrix * Vector4(translation, 1));
	return Vector3(
		transformedTranslation.x,
		transformedTranslation.y,
		transformedTranslation.z);
}

void Transform::RecalculateMatrix() const
{
	Matrix4x4 const rotationMatrix = Matrix3x3(Quaternion(rotation));

	Matrix4x4 const scaleMatrix = glm::scale(Matrix4x4(1), scale);

	Matrix4x4 const translationMatrix = glm::translate(Matrix4x4(1), position);

	matrix = translationMatrix * rotationMatrix * scaleMatrix;
	inverseMatrix = glm::inverse(matrix);

	dirty = false;
}
} // namespace LibRay
