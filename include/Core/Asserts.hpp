#pragma once

#include "Logger.hpp"

#ifndef NDEBUG

#define ASSERTIONS_ENABLED 1

#endif

#if ASSERTIONS_ENABLED == 1

#if _MSC_VER
#include <intrin.h>
#define debugBreak() __debugbreak()
#else
#define debugBreak() __builtin_trap()
#endif

#define SASSERT(expr) if (!(expr)) { SFATAL("Assertion failed: '", #expr, "' (file ", __FILE__, ", line ", __LINE__, ')'); debugBreak(); }
#define SASSERT_MSG(expr, ...) if (!(expr)) { SFATAL("Assertion failed: '", #expr, "' (file ", __FILE__, ", line ", __LINE__, ')'); SFATAL(__VA_ARGS__); debugBreak(); }

#else

#define SASSERT(expr, ...)

#endif