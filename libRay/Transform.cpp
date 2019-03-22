#include "Transform.hpp"

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
, matrix(Matrix4x4::Identity())
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
	Vector3 translatedDirection;

	translatedDirection.x = direction.x * matrix.r00
		+ direction.y * matrix.r01
		+ direction.z * matrix.r02;

	translatedDirection.y = direction.x * matrix.r10
		+ direction.y * matrix.r11
		+ direction.z * matrix.r12;

	translatedDirection.z = direction.x * matrix.r20
		+ direction.y * matrix.r21
		+ direction.z * matrix.r22;

	return translatedDirection;
}

Vector3 Transform::TransformTranslation(
	Matrix4x4 const &matrix,
	Vector3 const &translation)
{
	Vector3 translatedPosition;

	translatedPosition.x = translation.x * matrix.r00
		+ translation.y * matrix.r01
		+ translation.z * matrix.r02
		+ matrix.r03;

	translatedPosition.y = translation.x * matrix.r10
		+ translation.y * matrix.r11
		+ translation.z * matrix.r12
		+ matrix.r13;

	translatedPosition.z = translation.x * matrix.r20
		+ translation.y * matrix.r21
		+ translation.z * matrix.r22
		+ matrix.r23;

	float const w = translation.x * matrix.r30
		+ translation.y * matrix.r31
		+ translation.z * matrix.r32
		+ matrix.r33;

	translatedPosition /= w;

	return translatedPosition;
}

void Transform::RecalculateMatrix() const
{
	Matrix4x4 rotationMatrix = Matrix4x4::Identity()
		.RotateZ(rotation.z)
		.RotateY(rotation.y)
		.RotateX(rotation.x);

	Matrix4x4 scaleMatrix = Matrix4x4::Identity().Scale(scale);

	Matrix4x4 translationMatrix = Matrix4x4::Identity().Translate(position);

	matrix = translationMatrix * scaleMatrix * rotationMatrix;
	inverseMatrix = matrix.Inverted();

	dirty = false;
}
} // namespace LibRay
