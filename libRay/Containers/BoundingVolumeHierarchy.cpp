#include "BoundingVolumeHierarchy.hpp"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <numeric>

#include "../Math/MathUtils.hpp"
#include "../Math/Vector2.hpp"
#include "../Intersection.hpp"
#include "BoundingBox.hpp"

namespace LibRay::Containers
{
using namespace LibRay::Math;

using BVHDetails::ShapeVec;
using BVHDetails::IndexType;

BVH::BVH(ShapeVec &&objects)
: rootNode()
{
	MakeNodes(std::move(objects));
}

std::optional<Intersection> BVH::Traverse(Ray const &ray) const
{
	if(!rootNode)
		return std::nullopt;

	if(rootNode->Intersects(ray) >= 0.f)
		return rootNode->Traverse(ray);

	return std::nullopt;
}

BoundingBox BVH::CalculateBoundingBox(ShapeVec const &objects) const
{
	Vector3 min(FLT_MAX, FLT_MAX, FLT_MAX);
	Vector3 max(-FLT_MAX, -FLT_MAX, -FLT_MAX);

	for(Observer<Shapes::Shape const> const &object: objects)
	{
		BoundingBox boundingBox = object->CalculateBoundingBox();

		Vector3 const halfBoundaries = boundingBox.Dimensions() * 0.5f;

		min = min.Min(boundingBox.Position() - halfBoundaries);
		max = max.Max(boundingBox.Position() + halfBoundaries);
	}

	Vector3 const position = (max + min) * 0.5f;
	Vector3 const dimensions = (max - min).Abs();

	return BoundingBox(dimensions, position);
}

BoundingBox BVH::CalculateBoundingBox(
	BoundingBox const &a,
	BoundingBox const &b) const
{
	Vector3 const halfA = a.Dimensions() * 0.5f;
	Vector3 const halfB = b.Dimensions() * 0.5f;

	Vector3 const leftA = a.Position() - halfA;
	Vector3 const leftB = b.Position() - halfB;

	Vector3 const rightA = a.Position() + halfA;
	Vector3 const rightB = b.Position() + halfB;

	Vector3 const min = leftA.Min(rightA).Min(leftB).Min(rightB);
	Vector3 const max = leftA.Max(rightA).Max(leftB).Max(rightB);

	Vector3 const position = (max + min) * 0.5f;
	Vector3 const dimensions = (max - min).Abs();

	return BoundingBox(dimensions, position);
}

std::array<ShapeVec, 2> BVH::SplitObjects(ShapeVec &&objects) const
{
	auto const split = [&objects](bool x, bool y)
		-> std::pair<std::size_t, std::array<ShapeVec, 2>>
	{
		assert(!(x && y));

		ShapeVec sorted = objects;

		std::sort(
			sorted.begin(),
			sorted.end(),
			[x, y](
				ShapeVec::value_type const &a,
				ShapeVec::value_type const &b) -> bool
			{
				Vector3 const &positionA = a->Transform().Position();
				Vector3 const &positionB = b->Transform().Position();

				if(x)
					return positionA.x < positionB.x;
				else if(y)
					return positionA.y < positionB.y;

				return positionA.z < positionB.z;
			});

		Vector3 const &frontPos = sorted.front()->Transform().Position();
		Vector3 const &backPos = sorted.back()->Transform().Position();

		float const midX = frontPos.x + backPos.x;
		float const midY = frontPos.y + backPos.y;
		float const midZ = frontPos.z + backPos.z;

		float const mid = (x ? midX : y ? midY : midZ) * 0.5f;

		auto const midIt = std::upper_bound(
			sorted.cbegin(),
			sorted.cend(),
			mid,
			[x, y](float const midPoint, ShapeVec::value_type const &b) -> bool
			{
				Vector3 const position = b->Transform().Position();

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
			return {midPoint, {std::move(sorted), ShapeVec()}};
		}

		if(midIt == sorted.cbegin())
		{
			return {midPoint, {ShapeVec(), std::move(sorted)}};
		}

		ShapeVec left, right;

		left.reserve(midPoint);
		std::copy(sorted.cbegin(), midIt, std::back_inserter(left));

		right.reserve(std::size_t(std::distance(midIt, sorted.cend())));
		std::copy(midIt, sorted.cend(), std::back_inserter(right));

		return {midPoint, {std::move(left), std::move(right)}};
	};

	auto const xSplit = split(true, false);
	auto const ySplit = split(false, true);
	auto const zSplit = split(false, false);

	IndexType const size = objects.size();
	IndexType const midPoint = size / 2;
	IndexType const midPointX = xSplit.first;
	IndexType const midPointY = ySplit.first;
	IndexType const midPointZ = zSplit.first;

	IndexType const xDiff = midPointX > midPoint
		? midPointX - midPoint
		: midPoint - midPointX;

	IndexType const yDiff = midPointY > midPoint
		? midPointY - midPoint
		: midPoint - midPointY;

	IndexType const zDiff = midPointZ > midPoint
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

void BVH::MakeNodes(ShapeVec &&objects)
{
	using namespace BVHDetails;

	if(!objects.size())
		return;

	rootNode = std::make_unique<BVHNode>(*this, CalculateBoundingBox(objects));

	if(objects.size() <= leafSize)
	{
		rootNode->SetLeaf(std::make_unique<BVHLeaf>(std::move(objects)));
		return;
	}

	auto const generateChildren = [this](
		BVHNode &parent,
		bool left,
		ShapeVec &&objects,
		auto const &recurse) -> void
	{
		if(objects.size() <= leafSize)
		{
			auto child = std::make_unique<BVHNode>(
				*this,
				CalculateBoundingBox(objects));

			child->SetLeaf(std::make_unique<BVHLeaf>(std::move(objects)));

			if(left)
				parent.SetChild1(std::move(child));
			else
				parent.SetChild2(std::move(child));

			return;
		}

		auto child = std::make_unique<BVHNode>(
			*this,
			CalculateBoundingBox(objects));

		std::array<ShapeVec, 2> split = SplitObjects(std::move(objects));

		if(split[0].size() == 0 || split[1].size() == 0)
		{
			assert(!(split[0].size() == 0 && split[1].size() == 0));
			// Unable to make a proper split, just use this one as the leaf node
			// This should almost never happen.

			ShapeVec &selectedSplit = split[0].size() == 0
				? split[1]
				: split[0];

			auto leaf = std::make_unique<BVHNode>(
				*this,
				CalculateBoundingBox(selectedSplit));

			leaf->SetLeaf(std::make_unique<BVHLeaf>(std::move(selectedSplit)));

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

	std::array<ShapeVec, 2> split = SplitObjects(std::move(objects));
	generateChildren(*rootNode, true, std::move(split[0]), generateChildren);
	generateChildren(*rootNode, false, std::move(split[1]), generateChildren);
}

namespace BVHDetails
{
BVHNode::BVHNode(
	BVH const &master,
	BoundingBox &&boundingBox)
: BoundingBox(std::move(boundingBox))
, master(&master)
, isLeaf(false)
{
}

BVHNode::~BVHNode() noexcept
{
	if(isLeaf)
		child1.leaf.reset();
	else
		child1.node.reset();
}

std::optional<Intersection> BVHNode::Traverse(Ray const &ray) const
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
				distance = intersectionToOrigin.SquareMagnitude();

				if(distance < (boundingBoxDistance2 * boundingBoxDistance2))
					return intersection;
			}

			std::optional<Intersection> const intersection2 =
				child2->Traverse(ray);
			if(intersection && intersection2)
			{
				Vector3 const intersectionToOrigin =
					(intersection2->worldPosition - ray.Origin());
				float const distance2 = intersectionToOrigin.SquareMagnitude();

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
			distance = intersectionToOrigin.SquareMagnitude();

			if(distance < (boundingBoxDistance1 * boundingBoxDistance1))
				return intersection;
		}

		std::optional<Intersection> const intersection2 =
			child1.node->Traverse(ray);
		if(intersection && intersection2)
		{
			Vector3 const intersectionToOrigin =
				(intersection2->worldPosition - ray.Origin());
			float const distance2 = intersectionToOrigin.SquareMagnitude();

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

void BVHNode::SetLeaf(std::unique_ptr<BVHLeaf> leaf)
{
	isLeaf = true;
	child1.leaf = std::move(leaf);
}

void BVHNode::SetChild1(std::unique_ptr<BVHNode> node)
{
	assert(!isLeaf);

	child1.node = std::move(node);
}

void BVHNode::SetChild2(std::unique_ptr<BVHNode> node)
{
	assert(!isLeaf);

	child2 = std::move(node);
}

BVHLeaf::BVHLeaf(ShapeVec &&leafs)
: leafs(std::move(leafs))
{
}

std::optional<Intersection> BVHLeaf::Traverse(Ray const &ray) const
{
	std::optional<Intersection> closestIntersection;
	float closestDistance = 0;

	for(ShapeVec::value_type const &leaf: leafs)
	{
		std::optional<Intersection> intersection = leaf->Intersects(ray);

		if(!intersection)
			continue;

		Vector3 const intersectionToOrigin =
			(intersection->worldPosition - ray.Origin());
		float const distance = intersectionToOrigin.SquareMagnitude();

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
} // namespace BVHDetails
} // namespace LibRay::Containers
