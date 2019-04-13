#include <glm/vec3.hpp>

#if defined(_WIN32) && defined(UNICODE)
extern "C" int wmain();

extern "C" int wmain()
#else
int main()
#endif
{
	glm::vec3 f(1.0f, 2.0f, 3.0f);
	(void)f;

	return 0;
}
