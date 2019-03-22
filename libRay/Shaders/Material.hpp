#ifndef bde73d94_8f91_4914_72b1_67e1df6a9468
#define bde73d94_8f91_4914_72b1_67e1df6a9468

#include <map>
#include <string>
#include <type_traits>

#include "../Shapes/Color.hpp"
#include "../API.hpp"

namespace LibRay
{
class Shader;

class LIBRAY_API Material final
{
public:
	Material(Shader const &shader, float reflectiveness = 0.f);
	Material(Material const &) = default;
	Material(Material &&) = default;

	Material &operator=(Material const &) = delete;
	Material &operator=(Material &&) = delete;

	void UpdateFloatProperty(std::string const &name, float value);
	float FloatPropertyByName(std::string const &name) const;

	void Reflectiveness(float newReflectiveness);
	float Reflectiveness() const;

	void UpdateColorProperty(
		std::string const &name,
		Shapes::Color const &color);
	Shapes::Color const &ColorPropertyByName(std::string const &name) const;

	Shader const &Shader() const;

private:
	class Shader const &shader;
	std::map<std::string, float> floatProperties;
	std::map<std::string, Shapes::Color> colorProperties;

	float reflectiveness;
};

static_assert(std::is_copy_constructible_v<Material>);
static_assert(!std::is_copy_assignable_v<Material>);
static_assert(!std::is_trivially_copyable_v<Material>);

static_assert(std::is_move_constructible_v<Material>);
static_assert(!std::is_move_assignable_v<Material>);
} // namespace LibRay

#endif // bde73d94_8f91_4914_72b1_67e1df6a9468
