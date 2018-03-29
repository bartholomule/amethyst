#pragma once

#define AMETHYST_EPSILON (1e-9)
#define AMETHYST_EPS_SRQ (AMETHYST_EPSILON * AMETHYST_EPSILON)
#if !defined(M_PI)
#define M_PI 3.14159265358979323846
#endif

#include <limits.h>
#include <limits>

// This may need to be much lower for everything (huge number)
#define AMETHYST_DEPTH_MAX (std::numeric_limits<long>::max())

