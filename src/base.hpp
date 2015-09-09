#ifndef BASE_HPP__
#define BASE_HPP__

#if defined(HAS_GLES)
#include <GLES2/gl2.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <assert.h>
#define TO_STRING(...) #__VA_ARGS__
#else
#include <GL/glew.h>
#endif

#include <iostream>
#include <algorithm>
#include <string>
#include <fstream>
#include <streambuf>
#include <ctime>
#include <math.h>
#include <complex.h>
#include <limits.h>
#include <fstream>
#include <sstream>
#include <random>

#include <util/timeutils.h>

#ifndef DEBUG
#define FUNCTION_ENTER
#else
#define FUNCTION_ENTER \
    struct __funcDbg { \
        const char* funcname; \
          __funcDbg(const char* fn) : funcname(fn) { cout << "BEGIN: " << funcname << endl; };\
          ~__funcDbg() { cout << "END: " << funcname << endl; };\
    } __funcDbgRai(__PRETTY_FUNCTION__);
#endif

#endif 
