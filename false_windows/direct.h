#ifdef _WIN32

#include_next <direct.h>

#else

#ifndef _FAKE_DIRECT_H
#define _FAKE_DIRECT_H

#include <unistd.h>
#include <stdint.h>

static inline char *_getcwd(char *buffer, int max_size) {
	return getcwd(buffer, (size_t)max_size);
}

static inline int _chdir(const char *path) {
	return chdir(path);
}

#endif

#endif
