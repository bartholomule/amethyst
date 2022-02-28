#pragma once

#include <cstdint>
#include <cstring>

#ifdef _WIN32
#ifndef WINDOWS
#define WINDOWS
#endif
#else
#define POSIX
#endif

#if defined(linux) || defined(__linux__)
#define LINUX
#endif

#ifdef __APPLE__
#define OSX
#ifndef BSD
#define BSD
#endif
#endif

#ifdef __FreeBSD__
#define FREEBSD
#define BSD
#endif

//
// Support for constexpr and noexcept
//

// noexcept isn't supported until after VS2013, but other newer compilers on linux will require it in some cases.
#if defined(__GNUC__)
#define NOEXCEPT noexcept
#define CONSTEXPR_SUPPORTED
#elif defined(_MSC_FULL_VER)
#if _MSC_FULL_VER > 190023026
#define NOEXCEPT noexcept
#endif
#elif defined(__clang__)
#if __has_feature(cxx_noexcept)
#define NOEXCEPT noexcept
#endif
#if __has_feature(cxx_constexpr)
#define CONSTEXPR_SUPPORTED
#endif
#endif

#if !defined(NOEXCEPT)
#define NOEXCEPT
#endif

// See http://en.cppreference.com/w/cpp/experimental/feature_test
#if !defined(CONSTEXPR_SUPPORTED)
#if defined(__cpp_constexpr) && (__cpp_constexpr >= 201304)
#define CONSTEXPR_SUPPORTED
#endif
#endif

#if defined(CONSTEXPR_SUPPORTED)
#define CONSTEXPR_VAR constexpr
#define CONSTEXPR_FN constexpr
#else
#define CONSTEXPR_VAR const
#define CONSTEXPR_FN
#endif

//
// Detect 32 vs 64-bit.
//
#if UINTPTR_MAX == 0xFFFFFFFF
#define HOST_MACHINE_X32 true
#elif UINTPTR_MAX == 0xFFFFFFFFFFFFFFFF
#define HOST_MACHINE_X64 true
#else
#error Uknown host binary size
#endif


#if defined(WINDOWS)
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

using ssize_t = size_t;
#endif

#if defined(LINUX)
#include <unistd.h>
#endif

//
// Undefine stupid macros
//
#if defined(min)
#undef min
#endif
#if defined(max)
#undef max
#endif
