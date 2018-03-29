#pragma once

#include "unit_line_base.hpp"
#include "point3.hpp"
#include "line3.hpp"
namespace amethyst
{
    template <typename T>
    using unit_line3 = unit_line_base<T, point3<T>, vector3<T>>;
}
