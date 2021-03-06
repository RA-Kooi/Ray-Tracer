#ifndef bde73d94_8f91_4914_72b1_67e1df6a9468
#define bde73d94_8f91_4914_72b1_67e1df6a9468

#include <unordered_map>
#include <string>
#include <type_traits>

#include "../API.hpp"
#include "Color.hpp"
#include "Texture.hpp"

namespace LibRay
{
class Shader;
}

namespace LibRay::Materials
{
class LIBRAY_API Material final
{
public:
	Material(
		class Shader const &shader,
		float reflectiveness = 0.f,
		float refractiveIndexInside = 0.f,
		float refractiveIndexOutside = 1.f);

	Material(Material const &) = default;
	Material(Material &&) = default;

	class Shader const &Shader() const;

	void UpdateFloatProperty(std::string const &name, float value);
	float FloatPropertyByName(std::string const &name) const;

	void UpdateColorProperty(
		std::string const &name,
		Color const &color);
	Color const &ColorPropertyByName(std::string const &name) const;

	void UpdateTextureProperty(std::string const &name, Texture texture);
	Texture const &TexturePropertyByName(std::string const &name) const;

	void Reflectiveness(float newReflectiveness);
	float Reflectiveness() const;

	void RefractiveIndexInside(float newRefractiveIndexInside);
	float RefractiveIndexInside() const;

	void RefractiveIndexOutside(float newRefractiveIndexOutside);
	float RefractiveIndexOutside() const;

private:
	class Shader const &shader;
	std::unordered_map<std::string, float> floatProperties;
	std::unordered_map<std::string, Color> colorProperties;
	std::unordered_map<std::string, Texture> textureProperties;

	float reflectiveness;
	float refractiveIndexInside, refractiveIndexOutside;
};

static_assert(std::is_copy_constructible_v<Material>);
static_assert(!std::is_copy_assignable_v<Material>);
static_assert(!std::is_trivially_copyable_v<Material>);

static_assert(std::is_move_constructible_v<Material>);
static_assert(!std::is_move_assignable_v<Material>);
} // namespace LibRay::Materials

#endif // bde73d94_8f91_4914_72b1_67e1df6a9468
