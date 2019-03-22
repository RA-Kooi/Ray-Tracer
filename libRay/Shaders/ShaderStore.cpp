#include "ShaderStore.hpp"

#include <cstdio>
#include <stdexcept>

#include "Shader.hpp"

namespace LibRay
{
ShaderStore::~ShaderStore() noexcept = default;

Shader const &ShaderStore::AddShader(
	std::string const &name,
	std::unique_ptr<Shader> shader)
{
	auto it = shaders.find(name);

	if(it != shaders.end())
	{
		std::fprintf(
			stderr,
			"Warning: Replacing shader with name \"%s\".",
			name.c_str());

		it->second = std::move(shader);

		return *it->second;
	}

	return *shaders.insert(std::make_pair(name, std::move(shader))).first->second;
}

Shader const &ShaderStore::ShaderByName(std::string const &name) const
{
	using namespace std::string_literals;

	auto const it = shaders.find(name);

	if(it == shaders.cend())
	{
		throw std::runtime_error(
			"Unable to find shader with name "s + name + ".");
	}

	return *it->second;
}
} // namespace LibRay
