#pragma once

#include "Logger.hpp"

#ifndef NDEBUG

#define ASSERTIONS_ENABLED 1

#endif

#if ASSERTIONS_ENABLED == 1

#ifdef _MSC_VER
#include <intrin.h>
#define debugBreak() __debugbreak()
#else
//#define debugBreak() __builtin_trap()
#include <signal.h>
#define debugBreak() raise(SIGTRAP)
#endif

#define SASSERT(expr) if (!(expr)) { SFATAL("Assertion failed: '", #expr, "' in file ", __FILE__, " on line ", __LINE__); debugBreak(); }
#define SASSERT_MSG(expr, ...) if (!(expr)) { SFATAL("Assertion failed: '", #expr, "' in file ", __FILE__, " on line ", __LINE__); SFATAL(__VA_ARGS__); debugBreak(); }

#else

#define SASSERT(expr, ...)

#endif