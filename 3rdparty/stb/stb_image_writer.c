#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

void DisableTGARLE(void);
void DisableTGARLE(void)
{
	stbi_write_tga_with_rle = 0;
}
