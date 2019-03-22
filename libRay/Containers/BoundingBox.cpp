#include "BoundingBox.hpp"

namespace LibRay::Containers
{
using namespace LibRay::Math;

BoundingBox::BoundingBox(
	Vector3 const &dimensions,
	Vector3 const &position)
: dimensions(dimensions)
, position(position)
{
}

float BoundingBox::Intersects(Ray const &ray) const
{
	Vector3 const halfBoundaries = dimensions * 0.5f;

	Vector3 const left = (position - halfBoundaries);
	Vector3 const right = (position + halfBoundaries);

	Vector3 const minPoint = left.Min(right);
	Vector3 const maxPoint = left.Max(right);

	Vector3 const inverseDir = 1.f / ray.Direction();
	Vector3 const hit1 = (minPoint - ray.Origin()) * inverseDir;
	Vector3 const hit2 = (maxPoint - ray.Origin()) * inverseDir;

	Vector3 const min = hit1.Min(hit2);
	Vector3 const max = hit1.Max(hit2);

	float const near = min.MaxComponent();
	float const far = max.MinComponent();

	if(far < near)
		return -1.f;

	float distance = near;
	if(distance < 0)
	{
		distance = far;

		if(distance < 0)
			return -1.f;
	}

	return distance;
}

Vector3 const &BoundingBox::Dimensions() const
{
	return dimensions;
}

Vector3 const &BoundingBox::Position() const
{
	return position;
}
} // namespace LibRay::Containers
