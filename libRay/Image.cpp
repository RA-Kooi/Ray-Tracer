#include "Image.hpp"

namespace LibRay
{
Image::Image(std::size_t sizeX, std::size_t sizeY)
: sizeX(sizeX)
, sizeY(sizeY)
, pixels()
{
	pixels.reserve(sizeX * sizeY);
}
} // namespace LibRay
