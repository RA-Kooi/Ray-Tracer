#ifndef c7c0c195_96ac_8e78_a87e_4858fbc0fcce
#define c7c0c195_96ac_8e78_a87e_4858fbc0fcce

#include <map>
#include <memory>
#include <type_traits>
#include <string>

#include "../API.hpp"

namespace LibRay
{
class Shader;

class LIBRAY_API ShaderStore final
{
public:
	ShaderStore() = default;
	ShaderStore(ShaderStore const &) = delete;
	ShaderStore(ShaderStore &&) = default;
	~ShaderStore() noexcept;

	ShaderStore &operator=(ShaderStore const &) = delete;
	ShaderStore &operator=(ShaderStore &&) = default;

	Shader const &AddShader(
		std::string const &name,
		std::unique_ptr<Shader> shader);

	Shader const &ShaderByName(std::string const &name) const;

private:
	std::map<std::string, std::unique_ptr<Shader>> shaders;
};

static_assert(!std::is_copy_constructible_v<ShaderStore>);
static_assert(!std::is_copy_assignable_v<ShaderStore>);
static_assert(!std::is_trivially_copyable_v<ShaderStore>);

static_assert(std::is_move_constructible_v<ShaderStore>);
static_assert(std::is_move_assignable_v<ShaderStore>);
} // namespace LibRay

#endif // c7c0c195_96ac_8e78_a87e_4858fbc0fcce
