#if !defined(AMETHYST__TYPES_HPP)
#define AMETHYST__TYPES_HPP

#include "amethyst/config.hpp"

namespace amethyst
{
	typedef unsigned char UInt8;
	typedef signed char Int8;
#if SIZEOF_SHORT_INT == 2
	typedef unsigned short UInt16;
	typedef short Int16;
#define INT16_IS_SHORT 1
#elif SIZEOF_INT == 2
	typedef unsigned int UInt16;
	typedef int Int16;
#define INT16_IS_INT 1
#endif
#if SIZEOF_INT == 4
	typedef unsigned int UInt32;
	typedef int Int32;
#define INT32_IS_INT 1
#elif SIZEOF_LONG_INT == 4
	typedef unsigned long UInt32;
	typedef long Int32;
#define INT32_IS_LONG 1
#endif
#if SIZEOF_LONG_INT == 8
	typedef unsigned long UInt64;
	typedef long Int64;
#define INT64_IS_LONG 1
#elif SIZEOF_LONG_LONG_INT == 8
	typedef unsigned long long UInt64;
	typedef long long Int64;
#define INT64_IS_LONG_LONG 1
#else
#error "Compiler must support 64 bit long"
#endif
#if SIZEOF_DOUBLE == 8
	typedef double Real64;
#define REAL64_IS_DOUBLE 1
#elif SIZEOF_LONG_DOUBLE == 8
	typedef long double Real64;
#define REAL64_IS_LONG_DOUBLE 1
#endif
#if SIZEOF_FLOAT == 4
	typedef float Real32;
#define REAL32_IS_FLOAT 1
#elif SIZEOF_DOUBLE == 4
	typedef double Real32;
#define REAL32_IS_DOUBLE 1
#endif
} // namespace amethyst

#endif /* !defined(AMETHYST__TYPES_HPP) */
