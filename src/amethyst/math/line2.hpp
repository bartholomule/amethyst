#pragma once

#include "line_base.hpp"
#include "point2.hpp"
#include "vector2.hpp"

namespace amethyst
{
    template <typename T>
    using line2 = line_base<T, point2<T>, vector2<T>>;
}
