#include <boost/optional.hpp>

#if defined(_WIN32) && defined(UNICODE)
extern "C" int wmain();

extern "C" int wmain()
#else
int main()
#endif
{
	boost::optional<int> opt;
	(void)opt;

	return 0;
}
