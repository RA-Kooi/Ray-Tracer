#include <thread>

void thread();
void thread()
{
}

#if defined(_WIN32) && defined(UNICODE)
extern "C" int wmain();

extern "C" int wmain()
#else
int main()
#endif
{
	std::thread t(&thread);
	(void)t;

	return 0;
}
