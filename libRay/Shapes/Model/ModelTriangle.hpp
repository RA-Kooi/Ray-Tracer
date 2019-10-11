#ifndef d1b8dfca_af87_7379_5d37_8653d061fc40
#define d1b8dfca_af87_7379_5d37_8653d061fc40

#include <array>
#include <optional>
#include <type_traits>

#include "../../Material/MaterialStore.hpp"
#include "../../Math/Vector.hpp"
#include "../../API.hpp"
#include "../../Utilites.hpp"
#include "../Shape.hpp"

namespace LibRay
{
namespace Containers
{
class BoundingBox;
} // namespace Containers

namespace Math
{
class Ray;
} // namespace Math

class Intersection;

namespace Shapes
{
class Model;

class ModelTriangle final: public BaseShape<ModelTriangle>
{
public:
	struct Vertex
	{
		Math::Vector3 position, normal;
		Math::Vector2 uv;
	};

	static_assert(std::is_copy_constructible_v<Vertex>);
	static_assert(std::is_copy_assignable_v<Vertex>);
	static_assert(std::is_trivially_copyable_v<Vertex>);

	static_assert(std::is_move_constructible_v<Vertex>);
	static_assert(std::is_move_assignable_v<Vertex>);

public:
	ModelTriangle(Observer<Model const> parent, std::array<Vertex, 3> vertices);

	std::optional<Intersection> Intersects(Math::Ray const &ray) const override;

	Containers::BoundingBox CalculateBoundingBox() const override;

private:
	Math::Vector3 const PositionInternal() const;
	friend class BaseShape<ModelTriangle>;

private:
	Observer<Model const> parent;
	std::array<Vertex, 3> vertices;
};

extern template class BaseShape<ModelTriangle>;

static_assert(std::is_copy_constructible_v<ModelTriangle>);
static_assert(std::is_copy_assignable_v<ModelTriangle>);
static_assert(!std::is_trivially_copyable_v<ModelTriangle>);

static_assert(std::is_move_constructible_v<ModelTriangle>);
static_assert(std::is_move_assignable_v<ModelTriangle>);
} // namespace Shapes
} // namespace LibRay

#endif // d1b8dfca_af87_7379_5d37_8653d061fc40
