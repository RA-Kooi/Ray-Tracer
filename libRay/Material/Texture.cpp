#include "Texture.hpp"

#include <cmath>
#include <cstring>
#include <stdexcept>

#include <stb/stb_image.h>

#include "../Math/MathUtils.hpp"

namespace LibRay::Materials
{
using namespace LibRay::Math;

Texture::Texture(
	std::string const &fileName,
	WrappingMethod wrapMethodU,
	WrappingMethod wrapMethodV)
: dimensions(0, 0)
, rgbData()
, wrapMethodU(wrapMethodU)
, wrapMethodV(wrapMethodV)
{
	int width, height, channels;
	std::uint8_t *const data = stbi_load(
		fileName.c_str(),
		&width,
		&height,
		&channels,
		3);

	if(!data)
	{
		throw std::runtime_error(
			"Unable to load <"
			+ fileName
			+ ">, stb error reason: "
			+ std::string(stbi_failure_reason())
			+ ", errno: "
			+ std::string(strerror(errno)));
	}

	std::fprintf(stderr, "Notice: <%s> Channels: %d\n", fileName.c_str(), channels);

	new (this) Texture(
		{size_t(width), size_t(height)},
		data,
		InputFormat(channels),
		wrapMethodU,
		wrapMethodV);

	stbi_image_free(data);
}

Texture::Texture(
	Vector2st const &dimensions,
	std::uint8_t const *const rawData,
	InputFormat inputFormat,
	WrappingMethod wrapMethodU,
	WrappingMethod wrapMethodV)
: dimensions(dimensions)
, rgbData()
, wrapMethodU(wrapMethodU)
, wrapMethodV(wrapMethodV)
{
	std::uint8_t rPos = 0, gPos = 1, bPos = 2;
	std::size_t move = size_t(inputFormat);

	switch(inputFormat)
	{
		case InputFormat::GRAYSCALE:
		{
			rPos = gPos = bPos = 0;
			move = 3;
		} break;
		default: break;
	}

	rgbData.reserve(dimensions.x * dimensions.y);

	for(
		std::size_t i = 0;
		i < dimensions.x * dimensions.y * move;
		i += move)
	{
		float const r = float(*(rawData + i + rPos)) / 255.f;
		float const g = float(*(rawData + i + gPos)) / 255.f;
		float const b = float(*(rawData + i + bPos)) / 255.f;

		rgbData.emplace_back(r, g, b);
	}
}

Texture::Texture(
	Vector2st const &dimensions,
	std::vector<Color> rgbData,
	WrappingMethod wrapMethodU,
	WrappingMethod wrapMethodV)
: dimensions(dimensions)
, rgbData(std::move(rgbData))
, wrapMethodU(wrapMethodU)
, wrapMethodV(wrapMethodV)
{
}

void Texture::WrapMethodU(WrappingMethod newMethod)
{
	wrapMethodU = newMethod;
}

Texture::WrappingMethod Texture::WrapMethodU() const
{
	return wrapMethodU;
}

void Texture::WrapMethodV(WrappingMethod newMethod)
{
	wrapMethodV = newMethod;
}

Texture::WrappingMethod Texture::WrapMethodV() const
{
	return wrapMethodV;
}

Color const &Texture::Sample(float u, float v) const
{
	switch(wrapMethodU)
	{
	case WrappingMethod::Repeat:
	{
		float const absU = std::abs(u);
		u = absU - int(absU);
	} break;
	case WrappingMethod::Clamp:
	{
		u = Math::Clamp(u, 0.f, 1.f);
	} break;
	}

	switch(wrapMethodV)
	{
	case WrappingMethod::Repeat:
	{
		float const absV = std::abs(v);
		v = absV - int(absV);
	} break;
	case WrappingMethod::Clamp:
	{
		v = Math::Clamp(v, 0.f, 1.f);
	} break;
	}

	std::size_t x = std::size_t(std::round(u * (float(dimensions.x) - 1.f)));
	std::size_t y = std::size_t(std::round(v * (float(dimensions.y) - 1.f)));

	return rgbData[x + y * dimensions.x];
}

Texture const &Texture::Black()
{
	static Texture texture(
		{1, 1},
		{Color::Black()},
		WrappingMethod::Clamp,
		WrappingMethod::Clamp);

	return texture;
}

Texture const &Texture::Blue()
{
	static Texture texture(
		{1, 1},
		{Color::Blue()},
		WrappingMethod::Clamp,
		WrappingMethod::Clamp);

	return texture;
}

Texture const &Texture::Green()
{
	static Texture texture(
		{1, 1},
		{Color::Green()},
		WrappingMethod::Clamp,
		WrappingMethod::Clamp);

	return texture;
}

Texture const &Texture::Red()
{
	static Texture texture(
		{1, 1},
		{Color::Red()},
		WrappingMethod::Clamp,
		WrappingMethod::Clamp);

	return texture;
}

Texture const &Texture::White()
{
	static Texture texture(
		{1, 1},
		{Color::White()},
		WrappingMethod::Clamp,
		WrappingMethod::Clamp);

	return texture;
}
} // namespace LibRay::Materials
