#include "Material.hpp"

#include <cstdio>

#include "../Math/MathUtils.hpp"

namespace LibRay::Materials
{
Material::Material(
	class Shader const &shader,
	float reflectiveness,
	float refractiveIndexInside,
	Color const &absorbtion,
	float refractiveIndexOutside)
: shader(shader)
, floatProperties()
, colorProperties()
, textureProperties()
, reflectiveness(Math::Clamp(reflectiveness, 0.f, 1.f))
, refractiveIndexInside(refractiveIndexInside)
, refractiveIndexOutside(refractiveIndexOutside)
, absorbtion(absorbtion)
{
}

class Shader const &Material::Shader() const
{
	return shader;
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
			"Warning: Unable to find named float property \"%s\".\n",
			name.c_str());

		return 0.f;
	}

	return it->second;
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
			"Warning: Unable to find named color property \"%s\".\n",
			name.c_str());

		static Color black = Color::Black();
		return black;
	}

	return it->second;
}

void Material::UpdateTextureProperty(std::string const &name, Texture texture)
{
	auto const it = textureProperties.find(name);

	if(it != textureProperties.end())
	{
		it->second = std::move(texture);
		return;
	}

	textureProperties.emplace(name, std::move(texture));
}

Texture const &Material::TexturePropertyByName(std::string const &name) const
{
	auto const it = textureProperties.find(name);

	if(it == textureProperties.cend())
	{
		std::fprintf(
			stderr,
			"Warning: Unable to find named texture property \"%s\".\n",
			name.c_str());

		return Texture::Black();
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

void Material::RefractiveIndexInside(float newRefractiveIndexInside)
{
	refractiveIndexInside = newRefractiveIndexInside;
}

float Material::RefractiveIndexInside() const
{
	return refractiveIndexInside;
}

void Material::RefractiveIndexOutside(float newRefractiveIndexOutside)
{
	refractiveIndexOutside = newRefractiveIndexOutside;
}

float Material::RefractiveIndexOutside() const
{
	return refractiveIndexOutside;
}

Color const &Material::Absorbtion() const
{
	return absorbtion;
}

void Material::Absorbtion(Color const &newAbsorbtion)
{
	absorbtion = newAbsorbtion;
}
} // namespace LibRay::Materials
