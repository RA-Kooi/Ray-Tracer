#ifndef dc9791e3_8323_b52a_b580_71f02a7d3196
#define dc9791e3_8323_b52a_b580_71f02a7d3196

#include <type_traits>
#include <vector>

#include "../API.hpp"
#include "Material.hpp"

namespace LibRay::Materials
{
class LIBRAY_API MaterialStore final
{
public:
	using ContainerType = std::vector<Material>;
	using IndexType = ContainerType::size_type;

public:
	MaterialStore() noexcept = default;
	MaterialStore(MaterialStore const &) = default;
	MaterialStore(MaterialStore &&) noexcept = default;

	MaterialStore &operator=(MaterialStore const &) = delete;
	MaterialStore &operator=(MaterialStore &&) = default;

	IndexType AddMaterial(Material &&material);
	Material const &MaterialByIndex(IndexType index) const;

private:
	ContainerType materials;
};

static_assert(std::is_copy_constructible_v<MaterialStore>);
static_assert(!std::is_copy_assignable_v<MaterialStore>);
static_assert(!std::is_trivially_copyable_v<MaterialStore>);

static_assert(std::is_move_constructible_v<MaterialStore>);
static_assert(std::is_move_assignable_v<MaterialStore>);
} // namespace LibRay::Materials

#endif // dc9791e3_8323_b52a_b580_71f02a7d3196
