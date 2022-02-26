#pragma once

#define AMETHYST_EPSILON_DOUBLE (1e-9)
#define AMETHYST_EPSILON_FLOAT (1e-6f)
#define AMETHYST_EPSILON AMETHYST_EPSILON_DOUBLE

#define AMETHYST_EPS_SRQ (AMETHYST_EPSILON * AMETHYST_EPSILON)
#if !defined(M_PI)
#define M_PI 3.1415926535897932384626433832795028
#endif

#include <limits.h>
#include <limits>

// This may need to be much lower for everything (huge number)
#define AMETHYST_DEPTH_MAX (std::numeric_limits<long>::max())

