#ifndef BS_API_H
#define BS_API_H

#ifdef _WIN32
	#define BS_API __declspec(dllexport)
#else
	#define BS_API __attribute((visibility("default")))
#endif

#endif
