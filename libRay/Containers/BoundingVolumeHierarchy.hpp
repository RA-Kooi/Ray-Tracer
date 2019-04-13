#ifndef b3ae2d60_920d_38d8_4cdb_bf8f6ed2db14
#define b3ae2d60_920d_38d8_4cdb_bf8f6ed2db14

#include <array>
#include <memory>
#include <optional>
#include <type_traits>
#include <utility>
#include <vector>

#include "../Math/Ray.hpp"
#include "../Shapes/Shape.hpp"
#include "../API.hpp"
#include "../Utilites.hpp"
#include "BoundingBox.hpp"

namespace LibRay
{
class Intersection;

namespace Containers
{
class BVH;

namespace BVHDetails
{
constexpr std::size_t const leafSize = 2;

using ShapeVec = std::vector<Observer<Shapes::Shape const>>;
using IndexType = ShapeVec::size_type;

class BVHLeaf
{
public:
	BVHLeaf(ShapeVec &&leafs);

	std::optional<Intersection> Traverse(Math::Ray const &ray) const;

private:
	ShapeVec leafs;
};

static_assert(std::is_copy_constructible_v<BVHLeaf>);
static_assert(std::is_copy_assignable_v<BVHLeaf>);
static_assert(!std::is_trivially_copyable_v<BVHLeaf>);

static_assert(std::is_move_constructible_v<BVHLeaf>);
static_assert(std::is_move_assignable_v<BVHLeaf>);

class LIBRAY_API BVHNode final: public BoundingBox
{
public:
	BVHNode(BoundingBox &&boundingBox);

	~BVHNode() noexcept;

	std::optional<Intersection> Traverse(Math::Ray const &ray) const;

	void SetLeaf(std::unique_ptr<BVHLeaf> leaf);

	void SetChild1(std::unique_ptr<BVHNode> node);
	void SetChild2(std::unique_ptr<BVHNode> node);

private:
	bool isLeaf;

	union Node
	{
		Node()
		{
		}
		~Node() noexcept
		{
		}

		std::unique_ptr<BVHNode> node;
		std::unique_ptr<BVHLeaf> leaf;
	} child1;

	std::unique_ptr<BVHNode> child2;
};

static_assert(!std::is_copy_constructible_v<BVHNode>);
static_assert(!std::is_copy_assignable_v<BVHNode>);
static_assert(!std::is_trivially_copyable_v<BVHNode>);

static_assert(!std::is_move_constructible_v<BVHNode>);
static_assert(!std::is_move_assignable_v<BVHNode>);
} // namespace BVHDetails

class LIBRAY_API BVH final
{
public:
	explicit BVH(BVHDetails::ShapeVec &&objects);

	std::optional<Intersection> Traverse(Math::Ray const &ray) const;

private:
	BoundingBox CalculateBoundingBox(BVHDetails::ShapeVec const &objects) const;
	BoundingBox CalculateBoundingBox(
		BoundingBox const &a,
		BoundingBox const &b) const;

	std::array<BVHDetails::ShapeVec, 2> SplitObjects(
		BVHDetails::ShapeVec &&objects) const;

	void MakeNodes(BVHDetails::ShapeVec &&objects);

private:
	friend class BVHDetails::BVHNode;

	std::unique_ptr<BVHDetails::BVHNode> rootNode;
};

static_assert(!std::is_copy_constructible_v<BVH>);
static_assert(!std::is_copy_assignable_v<BVH>);
static_assert(!std::is_trivially_copyable_v<BVH>);

static_assert(std::is_move_constructible_v<BVH>);
static_assert(std::is_move_assignable_v<BVH>);
} // namespace Containers
} // namespace LibRay

#endif // b3ae2d60_920d_38d8_4cdb_bf8f6ed2db14
