#include "BoundingVolumeHierarchy.hpp"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstring>
#include <numeric>

#include "../Math/MathUtils.hpp"
#include "../Math/Vector.hpp"
#include "../Shapes/Model/ModelTriangle.hpp"
#include "../Intersection.hpp"
#include "BoundingBox.hpp"

namespace LibRay::Containers
{
using namespace LibRay::Math;

using BVHDetails::ShapeVec;
using BVHDetails::IndexType;

template<typename T>
BVH<T>::BVH(ShapeVec<T> &&objects)
: rootNode()
{
	MakeNodes(std::move(objects));
}

template<typename T>
std::optional<Intersection> BVH<T>::Traverse(Ray const &ray) const
{
	if(!rootNode)
		return std::nullopt;

	if(rootNode->Intersects(ray) >= 0.f)
		return rootNode->Traverse(ray);

	return std::nullopt;
}

template<typename T>
BoundingBox BVH<T>::RootBoundingBox() const
{
	if(!rootNode)
		return BoundingBox(Vector3(0), Vector3(0));

	return *rootNode;
}

template<typename T>
BoundingBox BVH<T>::CalculateBoundingBox(ShapeVec<T> const &objects) const
{
	Vector3 min(FLT_MAX, FLT_MAX, FLT_MAX);
	Vector3 max(-FLT_MAX, -FLT_MAX, -FLT_MAX);

	for(Observer<Shapes::BaseShape<T> const> const &object: objects)
	{
		BoundingBox boundingBox = object->CalculateBoundingBox();

		Vector3 const &halfBoundaries = boundingBox.HalfBoundaries();

		min = glm::min(min, boundingBox.Position() - halfBoundaries);
		max = glm::max(max, boundingBox.Position() + halfBoundaries);
	}

	Vector3 const position = (max + min) * 0.5f;

	return BoundingBox((max - min) * 0.5f, position);
}

template<typename T>
BoundingBox BVH<T>::CalculateBoundingBox(
	BoundingBox const &a,
	BoundingBox const &b) const
{
	Vector3 const halfA = a.HalfBoundaries();
	Vector3 const halfB = b.HalfBoundaries();

	Vector3 const leftA = a.Position() - halfA;
	Vector3 const leftB = b.Position() - halfB;

	Vector3 const rightA = a.Position() + halfA;
	Vector3 const rightB = b.Position() + halfB;

	Vector3 const min = glm::min(leftA, glm::min(rightA, glm::min(leftB, rightB)));
	Vector3 const max = glm::max(leftA, glm::max(rightA, glm::max(leftB, rightB)));

	Vector3 const position = (max + min) * 0.5f;

	return BoundingBox((max - min) * 0.5f, position);
}

template<typename T>
std::array<ShapeVec<T>, 2> BVH<T>::SplitObjects(ShapeVec<T> &&objects) const
{
	auto const split = [&objects](bool x, bool y)
		-> std::pair<std::size_t, std::array<ShapeVec<T>, 2>>
	{
		assert(!(x && y));

		ShapeVec<T> sorted = objects;

		std::sort(
			sorted.begin(),
			sorted.end(),
			[x, y](
				typename ShapeVec<T>::value_type const &a,
				typename ShapeVec<T>::value_type const &b) -> bool
			{
				Vector3 const &positionA = a->Position();
				Vector3 const &positionB = b->Position();

				if(x)
					return positionA.x < positionB.x;
				else if(y)
					return positionA.y < positionB.y;

				return positionA.z < positionB.z;
			});

		Vector3 const &frontPos = sorted.front()->Position();
		Vector3 const &backPos = sorted.back()->Position();

		float const midX = frontPos.x + backPos.x;
		float const midY = frontPos.y + backPos.y;
		float const midZ = frontPos.z + backPos.z;

		float const mid = (x ? midX : y ? midY : midZ) * 0.5f;

		auto const midIt = std::upper_bound(
			sorted.cbegin(),
			sorted.cend(),
			mid,
			[x, y](
				float const midPoint,
				typename ShapeVec<T>::value_type const &b) -> bool
			{
				Vector3 const position = b->Position();

				if(x)
					return position.x > midPoint;
				else if(y)
					return position.y > midPoint;

				return position.z > midPoint;
			});

		std::size_t midPoint =
			std::size_t(std::distance(sorted.cbegin(), midIt));

		if(midIt == sorted.cend())
		{
			return {midPoint, {std::move(sorted), ShapeVec<T>()}};
		}

		if(midIt == sorted.cbegin())
		{
			return {midPoint, {ShapeVec<T>(), std::move(sorted)}};
		}

		ShapeVec<T> left, right;

		left.reserve(midPoint);
		std::copy(sorted.cbegin(), midIt, std::back_inserter(left));

		right.reserve(std::size_t(std::distance(midIt, sorted.cend())));
		std::copy(midIt, sorted.cend(), std::back_inserter(right));

		return {midPoint, {std::move(left), std::move(right)}};
	};

	auto const xSplit = split(true, false);
	auto const ySplit = split(false, true);
	auto const zSplit = split(false, false);

	IndexType<T> const size = objects.size();
	IndexType<T> const midPoint = size / 2;
	IndexType<T> const midPointX = xSplit.first;
	IndexType<T> const midPointY = ySplit.first;
	IndexType<T> const midPointZ = zSplit.first;

	IndexType<T> const xDiff = midPointX > midPoint
		? midPointX - midPoint
		: midPoint - midPointX;

	IndexType<T> const yDiff = midPointY > midPoint
		? midPointY - midPoint
		: midPoint - midPointY;

	IndexType<T> const zDiff = midPointZ > midPoint
		? midPointZ - midPoint
		: midPoint - midPointZ;

	Observer<decltype(xSplit)> selectedSplit = nullptr;

	if((xDiff < yDiff && xDiff < zDiff)
	   || midPointX == midPoint)
		selectedSplit = &xSplit;
	else if((yDiff < xDiff && yDiff < zDiff)
			|| midPointY == midPoint)
		selectedSplit = &ySplit;
	else if((zDiff < xDiff && zDiff < yDiff)
			|| midPointZ == midPoint)
		selectedSplit = &zSplit;
	else // Very unlikely, all splits are the same
		selectedSplit = &xSplit;

	auto &leftRight = selectedSplit->second;

	return {std::move(leftRight[0]), std::move(leftRight[1])};
}

template<typename T>
void BVH<T>::MakeNodes(ShapeVec<T> &&objects)
{
	using namespace BVHDetails;

	if(!objects.size())
		return;

	rootNode = std::make_unique<BVHNode<T>>(CalculateBoundingBox(objects));

	if(objects.size() <= leafSize)
	{
		rootNode->SetLeaf(std::make_unique<BVHLeaf<T>>(std::move(objects)));
		return;
	}

	auto const generateChildren = [this](
		BVHNode<T> &parent,
		bool left,
		ShapeVec<T> &&objects,
		auto const &recurse) -> void
	{
		if(objects.size() <= leafSize)
		{
			auto child = std::make_unique<BVHNode<T>>(
				CalculateBoundingBox(objects));

			child->SetLeaf(std::make_unique<BVHLeaf<T>>(std::move(objects)));

			if(left)
				parent.SetChild1(std::move(child));
			else
				parent.SetChild2(std::move(child));

			return;
		}

		auto child = std::make_unique<BVHNode<T>>(CalculateBoundingBox(objects));

		std::array<ShapeVec<T>, 2> split = SplitObjects(std::move(objects));

		if(split[0].size() == 0 || split[1].size() == 0)
		{
			assert(!(split[0].size() == 0 && split[1].size() == 0));
			// Unable to make a proper split, just use this one as the leaf node
			// This should almost never happen.

			ShapeVec<T> &selectedSplit = split[0].size() == 0
				? split[1]
				: split[0];

			auto leaf = std::make_unique<BVHNode<T>>(
				CalculateBoundingBox(selectedSplit));

			leaf->SetLeaf(std::make_unique<BVHLeaf<T>>(std::move(selectedSplit)));

			if(left)
				parent.SetChild1(std::move(leaf));
			else
				parent.SetChild2(std::move(leaf));

			return;
		}

		recurse(*child, true, std::move(split[0]), recurse);
		recurse(*child, false, std::move(split[1]), recurse);

		if(left)
			parent.SetChild1(std::move(child));
		else
			parent.SetChild2(std::move(child));
	};

	std::array<ShapeVec<T>, 2> split = SplitObjects(std::move(objects));
	generateChildren(*rootNode, true, std::move(split[0]), generateChildren);
	generateChildren(*rootNode, false, std::move(split[1]), generateChildren);
}

template class BVH<Shapes::Shape>;
template class BVH<Shapes::ModelTriangle>;

namespace BVHDetails
{
template<typename T>
BVHNode<T>::BVHNode(BoundingBox &&boundingBox)
: BoundingBox(std::move(boundingBox))
, isLeaf(false)
, child1()
, child2()
{
	memset(&child1, 0, sizeof(decltype(child1)));
}

template<typename T>
BVHNode<T>::~BVHNode() noexcept
{
	if(isLeaf)
		child1.leaf.reset();
	else
		child1.node.reset();
}

template<typename T>
std::optional<Intersection> BVHNode<T>::Traverse(Ray const &ray) const
{
	if(isLeaf)
	{
		assert(child1.leaf);

		return child1.leaf->Traverse(ray);
	}

	// Now this is by far the best code I've ever written (lie).
	// It's long, but quite simple actually.
	// If we have 2 child nodes, we check if both bounding boxes intersect.
	// If not, return either one of the 2, and neither if both don't intersect.
	// If they both intersect, try the nearest bounding box first.
	// If then there's an intersection, see if it's closer than the other
	// bounding box. If it is, return it, if not, get the possible intersection
	// from the other child and return the nearest one.
	if(child1.node && child2)
	{
		float const boundingBoxDistance1 = child1.node->Intersects(ray);
		float const boundingBoxDistance2 = child2->Intersects(ray);

		if(boundingBoxDistance1 < 0.f && boundingBoxDistance2 < 0.f)
			return std::nullopt;

		if(boundingBoxDistance1 < 0.f)
			return child2->Traverse(ray);

		if(boundingBoxDistance2 < 0.f)
			return child1.node->Traverse(ray);

		if(boundingBoxDistance1 < boundingBoxDistance2)
		{
			std::optional<Intersection> const intersection =
				child1.node->Traverse(ray);

			float distance = 0.f;

			if(intersection)
			{
				Vector3 const intersectionToOrigin =
					(intersection->worldPosition - ray.Origin());
				distance = glm::length2(intersectionToOrigin);

				if(distance < (boundingBoxDistance2 * boundingBoxDistance2))
					return intersection;
			}

			std::optional<Intersection> const intersection2 =
				child2->Traverse(ray);
			if(intersection && intersection2)
			{
				Vector3 const intersectionToOrigin =
					(intersection2->worldPosition - ray.Origin());
				float const distance2 = glm::length2(intersectionToOrigin);

				if(distance < distance2)
					return intersection;
				else
					return intersection2;
			}

			if(intersection)
				return intersection;
			else
				return intersection2;
		}

		std::optional<Intersection> const intersection =
			child2->Traverse(ray);

		float distance = 0.f;

		if(intersection)
		{
			Vector3 const intersectionToOrigin =
				(intersection->worldPosition - ray.Origin());
			distance = glm::length2(intersectionToOrigin);

			if(distance < (boundingBoxDistance1 * boundingBoxDistance1))
				return intersection;
		}

		std::optional<Intersection> const intersection2 =
			child1.node->Traverse(ray);
		if(intersection && intersection2)
		{
			Vector3 const intersectionToOrigin =
				(intersection2->worldPosition - ray.Origin());
			float const distance2 = glm::length2(intersectionToOrigin);

			if(distance < distance2)
				return intersection;
			else
				return intersection2;
		}

		if(intersection)
			return intersection;
		else
			return intersection2;
	}

	if(child1.node)
	{
		if(child1.node->Intersects(ray) >= 0.f)
			return child1.node->Traverse(ray);
	}

	if(child2)
	{
		if(child2->Intersects(ray) >= 0.f)
			return child2->Traverse(ray);
	}

	return std::nullopt;
}

template<typename T>
void BVHNode<T>::SetLeaf(std::unique_ptr<BVHLeaf<T>> leaf)
{
	isLeaf = true;
	child1.leaf = std::move(leaf);
}

template<typename T>
void BVHNode<T>::SetChild1(std::unique_ptr<BVHNode<T>> node)
{
	assert(!isLeaf);

	child1.node = std::move(node);
}

template<typename T>
void BVHNode<T>::SetChild2(std::unique_ptr<BVHNode<T>> node)
{
	assert(!isLeaf);

	child2 = std::move(node);
}

template class BVHNode<Shapes::Shape>;
template class BVHNode<Shapes::ModelTriangle>;

template<typename T>
BVHLeaf<T>::BVHLeaf(ShapeVec<T> &&leafs)
: leafs(std::move(leafs))
{
}

template<typename T>
std::optional<Intersection> BVHLeaf<T>::Traverse(Ray const &ray) const
{
	std::optional<Intersection> closestIntersection;
	float closestDistance = 0;

	for(typename ShapeVec<T>::value_type const &leaf: leafs)
	{
		std::optional<Intersection> intersection = leaf->Intersects(ray);

		if(!intersection)
			continue;

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

	return closestIntersection;
}

template class BVHLeaf<Shapes::Shape>;
template class BVHLeaf<Shapes::ModelTriangle>;
} // namespace BVHDetails
} // namespace LibRay::Containers
