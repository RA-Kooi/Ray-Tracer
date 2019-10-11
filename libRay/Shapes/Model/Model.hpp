#ifndef d9b383bd_8be1_c54b_5e07_86257c3f18ec
#define d9b383bd_8be1_c54b_5e07_86257c3f18ec

#include <optional>
#include <string>
#include <type_traits>
#include <vector>

#include "../../Containers/BoundingVolumeHierarchy.hpp"
#include "../../Material/MaterialStore.hpp"
#include "../../API.hpp"
#include "../../Transform.hpp"
#include "../Shape.hpp"
#include "ModelTriangle.hpp"

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
class LIBRAY_API Model final: public Shape
{
public:
	Model(
		std::string const &fileName,
		bool invertNormalZ,
		class Transform const &transform,
		Materials::MaterialStore const &materialStore,
		Materials::MaterialStore::IndexType materialIndex);

	std::optional<Intersection> Intersects(Math::Ray const &ray) const override;

	Containers::BoundingBox CalculateBoundingBox() const override;

private:
	std::vector<Observer<BaseShape<ModelTriangle> const>> Load(
		std::string const &fileName,
		bool invertNormalZ);

private:
	std::vector<ModelTriangle> triangles;
	Containers::BVH<ModelTriangle> bvh;
};

static_assert(!std::is_copy_constructible_v<Model>);
static_assert(!std::is_copy_assignable_v<Model>);
static_assert(!std::is_trivially_copyable_v<Model>);

static_assert(std::is_move_constructible_v<Model>);
static_assert(!std::is_move_assignable_v<Model>);
} // namespace Shapes
} // namespace LibRay

#endif // d9b383bd_8be1_c54b_5e07_86257c3f18ec
