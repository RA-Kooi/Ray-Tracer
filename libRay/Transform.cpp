#include "Transform.hpp"

#include <Eigen/Geometry>

namespace LibRay
{
using namespace Math;

Transform::Transform()
: position(Vector3::Zero())
, scale(Vector3::Ones())
, rotation(Quaternion::Identity())
, dirty(false)
{
	RecalculateMatrix();
}

Transform::Transform(
	Vector3 const &position,
	Quaternion const &rotation,
	Vector3 const &scale)
: position(position)
, scale(scale)
, rotation(rotation)
, dirty(false)
{
	RecalculateMatrix();
}

Transform::Transform(
	Vector3 const &position,
	Eigen::AngleAxis<float> const &rotation,
	Vector3 const &scale)
: position(position)
, scale(scale)
, rotation(rotation)
, dirty(false)
{
	RecalculateMatrix();
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

void Transform::AddRotation(Math::Quaternion const &extraRotation)
{
	dirty = true;

	rotation *= extraRotation;
}

void Transform::Rotate(Math::Quaternion const &newRotation)
{
	dirty = true;

	rotation = newRotation;
}

Quaternion const &Transform::Rotation() const
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
	return matrix;
}

Matrix4x4 const &Transform::InverseMatrix() const
{
	return inverseMatrix;
}

Vector3 Transform::TransformDirection(
	Matrix4x4 const &matrix,
	Vector3 const &direction)
{
	return matrix.topLeftCorner<3, 3>() * direction;
}

Vector3 Transform::TransformTranslation(
	Matrix4x4 const &matrix,
	Vector3 const &translation)
{
	Vector4 const transformed = matrix
		* Vector4(translation.x(), translation.y(), translation.z(), 1);

	return transformed.block<3, 1>(0, 0);
}

void Transform::RecalculateMatrix()
{
	Matrix4x4 scaleMatrix = Matrix4x4::Identity();
	scaleMatrix.topLeftCorner<3, 3>() =
		Eigen::Scaling(scale.x(), scale.y(), scale.z());

	Matrix4x4 const rotationMatrix = Eigen::Affine3f(rotation).matrix();

	Matrix4x4 translationMatrix = Matrix4x4::Identity();
	translationMatrix.col(3).head<3>() = position;

	matrix = translationMatrix * rotationMatrix * scaleMatrix;
	inverseMatrix = matrix.inverse();

	dirty = false;
}

bool Transform::IsDirty() const
{
	return dirty;
}
} // namespace LibRay
