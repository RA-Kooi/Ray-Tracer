#ifndef e647c7e8_c980_795d_be0d_2b0fe938636d
#define e647c7e8_c980_795d_be0d_2b0fe938636d

#include <cstdint>
#include <type_traits>
#include <vector>

#include "Material/Color.hpp"
#include "API.hpp"

namespace LibRay
{
class LIBRAY_API Image final
{
public:
	Image(std::size_t sizeX, std::size_t sizeY);

public:
	std::size_t sizeX, sizeY;
	std::vector<Shapes::Color> pixels;
};

static_assert(std::is_copy_constructible_v<Image>);
static_assert(std::is_copy_assignable_v<Image>);
static_assert(!std::is_trivially_copyable_v<Image>);

static_assert(std::is_move_constructible_v<Image>);
static_assert(std::is_move_assignable_v<Image>);
} // namespace LibRay

#endif // e647c7e8_c980_795d_be0d_2b0fe938636d
