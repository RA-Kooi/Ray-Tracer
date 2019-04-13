#include <GL/gl.h>

#if defined(_WIN32) && defined(UNICODE)
extern "C" int wmain();

extern "C" int wmain()
#else
int main()
#endif
{
	glClearColor(0, 0, 0, 0);

	return 0;
}
