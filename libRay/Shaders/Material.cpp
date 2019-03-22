#include "Material.hpp"

#include <cstdio>

#include "../Math/MathUtils.hpp"

namespace LibRay
{
using namespace Shapes;

Material::Material(class Shader const &shader, float reflectiveness)
: shader(shader)
, floatProperties()
, colorProperties()
, reflectiveness(Math::Clamp(reflectiveness, 0.f, 1.f))
{
}

void Material::UpdateFloatProperty(std::string const &name, float value)
{
	floatProperties[name] = value;
}

float Material::FloatPropertyByName(std::string const &name) const
{
	auto const it = floatProperties.find(name);

	if(it == floatProperties.cend())
	{
		std::fprintf(
			stderr,
			"Warning: Unable to find named property \"%s\".",
			it->first.c_str());

		return 0.f;
	}

	return it->second;
}

void Material::Reflectiveness(float newReflectiveness)
{
	reflectiveness = newReflectiveness;
}

float Material::Reflectiveness() const
{
	return reflectiveness;
}

void Material::UpdateColorProperty(std::string const &name, Color const &color)
{
	auto const it = colorProperties.find(name);

	if(it != colorProperties.end())
	{
		it->second = color;
		return;
	}

	colorProperties.emplace(name, color);
}

Color const &Material::ColorPropertyByName(std::string const &name) const
{
	auto const it = colorProperties.find(name);

	if(it == colorProperties.cend())
	{
		std::fprintf(
			stderr,
			"Warning: Unable to find named property \"%s\".",
			it->first.c_str());

		static Color black = Color::Black();
		return black;
	}

	return it->second;
}

class Shader const &Material::Shader() const
{
	return shader;
}
} // namespace LibRay
