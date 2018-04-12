#pragma once

#include <memory>

namespace amethyst
{
    template <typename T, typename color_type> class shape;
    template <typename T, typename color_type> class sphere;
    template <typename T, typename color_type> class plane;


    template <typename T, typename color_type>
    using shape_ptr = std::shared_ptr<shape<T, color_type>>;
}