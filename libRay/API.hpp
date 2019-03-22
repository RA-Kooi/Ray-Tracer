#ifndef d5b94e2c_f702_44b9_9789_ac371d14b25e
#define d5b94e2c_f702_44b9_9789_ac371d14b25e

#if defined(LIBRAY_EXPORTS) || defined(LIBRAY_AS_DLL)
	#ifdef LIBRAY_AS_DLL
		#ifdef _WIN32
			#define LIBRAY_API __declspec(dllimport)
		#else
			#define LIBRAY_API
		#endif
		#define LIBRAY_LOCAL
	#else
		#ifdef _WIN32
			#define LIBRAY_API __declspec(dllexport)
			#define LIBRAY_LOCAL
		#elif (defined(__GNUC__) && (__GNUC__ >= 4))
			#define LIBRAY_API __attribute__ ((visibility("default")))
			#define LIBRAY_LOCAL __attribute__ ((visibility("hidden")))
		#endif
	#endif
#else
	#define LIBRAY_API
	#define LIBRAY_LOCAL
#endif

#endif // d5b94e2c_f702_44b9_9789_ac371d14b25e
