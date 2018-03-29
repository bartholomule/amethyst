#pragma once

#include "unit_line_base.hpp"
#include "point2.hpp"
#include "line2.hpp"
namespace amethyst
{
    template <typename T>
    using unit_line2 = unit_line_base<T, point2<T>, vector2<T>>;
}
