#pragma once

#include "line_base.hpp"
#include "point3.hpp"
#include "vector3.hpp"

namespace amethyst
{
    template <typename T>
    using line3 = line_base<T, point3<T>, vector3<T>>;
}
