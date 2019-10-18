#ifndef c2e98b7c_23a9_e0af_781f_59e73721ca8f
#define c2e98b7c_23a9_e0af_781f_59e73721ca8f

#include <memory>
#include <string>
#include <type_traits>
#include <vector>

#include "../../Material/MaterialStore.hpp"
#include "../../API.hpp"
#include "Model.hpp"

namespace LibRay
{
class Transform;

namespace Shapes
{
class LIBRAY_API ModelLoader final
{
public:
	ModelLoader(Materials::MaterialStore &materialStore);

	std::vector<std::unique_ptr<Model>> LoadObj(
		std::string const &fileName,
		Transform const &transform,
		std::string const &materialDir = ".",
		Materials::MaterialStore::IndexType materialIndex = 0,
		bool invertNormalZ = false) const;

private:
	Materials::MaterialStore &materialStore;
};

static_assert(std::is_copy_constructible_v<ModelLoader>);
static_assert(!std::is_copy_assignable_v<ModelLoader>);
#ifdef _MSC_VER
static_assert(!std::is_trivially_copyable_v<ModelLoader>);
#else
static_assert(std::is_trivially_copyable_v<ModelLoader>);
#endif

static_assert(std::is_move_constructible_v<ModelLoader>);
static_assert(!std::is_move_assignable_v<ModelLoader>);
} // namespace Shapes
} // namespace LibRay

#endif // c2e98b7c_23a9_e0af_781f_59e73721ca8f
