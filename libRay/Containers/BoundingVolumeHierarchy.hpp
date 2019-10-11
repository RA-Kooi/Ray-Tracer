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
namespace Shapes
{
class ModelTriangle;
} // namespace Shapes

class Intersection;

namespace Containers
{
template<typename T> class BVH;

namespace BVHDetails
{
constexpr std::size_t const leafSize = 2;

template<typename T>
using ShapeVec = std::vector<Observer<Shapes::BaseShape<T> const>>;

template<typename T>
using IndexType = typename ShapeVec<T>::size_type;


template<typename T>
class BVHLeaf final
{
public:
	BVHLeaf(ShapeVec<T> &&leafs);

	std::optional<Intersection> Traverse(Math::Ray const &ray) const;

private:
	ShapeVec<T> leafs;
};

extern template class BVHLeaf<Shapes::Shape>;
extern template class BVHLeaf<Shapes::ModelTriangle>;

static_assert(std::is_copy_constructible_v<BVHLeaf<Shapes::Shape>>);
static_assert(std::is_copy_assignable_v<BVHLeaf<Shapes::Shape>>);
static_assert(!std::is_trivially_copyable_v<BVHLeaf<Shapes::Shape>>);

static_assert(std::is_move_constructible_v<BVHLeaf<Shapes::Shape>>);
static_assert(std::is_move_assignable_v<BVHLeaf<Shapes::Shape>>);

template<typename T>
class LIBRAY_API BVHNode final: public BoundingBox
{
public:
	BVHNode(BoundingBox &&boundingBox);

	~BVHNode() noexcept;

	std::optional<Intersection> Traverse(Math::Ray const &ray) const;

	void SetLeaf(std::unique_ptr<BVHLeaf<T>> leaf);

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
		std::unique_ptr<BVHLeaf<T>> leaf;
	} child1;

	std::unique_ptr<BVHNode> child2;
};

extern template class BVHNode<Shapes::Shape>;
extern template class BVHNode<Shapes::ModelTriangle>;

static_assert(!std::is_copy_constructible_v<BVHNode<Shapes::Shape>>);
static_assert(!std::is_copy_assignable_v<BVHNode<Shapes::Shape>>);
static_assert(!std::is_trivially_copyable_v<BVHNode<Shapes::Shape>>);

static_assert(!std::is_move_constructible_v<BVHNode<Shapes::Shape>>);
static_assert(!std::is_move_assignable_v<BVHNode<Shapes::Shape>>);
} // namespace BVHDetails

template<typename T>
class LIBRAY_API BVH final
{
public:
	explicit BVH(BVHDetails::ShapeVec<T> &&objects);

	std::optional<Intersection> Traverse(Math::Ray const &ray) const;

	BoundingBox RootBoundingBox() const;

private:
	BoundingBox CalculateBoundingBox(BVHDetails::ShapeVec<T> const &objects) const;
	BoundingBox CalculateBoundingBox(
		BoundingBox const &a,
		BoundingBox const &b) const;

	std::array<BVHDetails::ShapeVec<T>, 2> SplitObjects(
		BVHDetails::ShapeVec<T> &&objects) const;

	void MakeNodes(BVHDetails::ShapeVec<T> &&objects);

private:
	friend class BVHDetails::BVHNode<T>;

	std::unique_ptr<BVHDetails::BVHNode<T>> rootNode;
};

extern template class BVH<Shapes::Shape>;
extern template class BVH<Shapes::ModelTriangle>;

static_assert(!std::is_copy_constructible_v<BVH<Shapes::Shape>>);
static_assert(!std::is_copy_assignable_v<BVH<Shapes::Shape>>);
static_assert(!std::is_trivially_copyable_v<BVH<Shapes::Shape>>);

static_assert(std::is_move_constructible_v<BVH<Shapes::Shape>>);
static_assert(std::is_move_assignable_v<BVH<Shapes::Shape>>);
} // namespace Containers
} // namespace LibRay

#endif // b3ae2d60_920d_38d8_4cdb_bf8f6ed2db14
