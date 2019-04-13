#if defined(_WIN32) && defined(UNICODE)
extern "C" int wmain();

extern "C" int wmain()
#else
int main()
#endif
{
	return 0;
}
