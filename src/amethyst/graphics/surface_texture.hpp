#pragma once

#include "amethyst/graphics/texture.hpp"

namespace amethyst
{
    /**
     *
     * A surface texture -- one that depends on u-v cordinates to produce its
     * colors.
     *
     * @author Kevin Harris <kpharris@users.sourceforge.net>
     * @version $Revision: 1.1 $
     *
     */
    template <class T, class color_type>
    class surface_texture : public texture<T, color_type>
    {
    public:
        surface_texture() = default;
        virtual ~surface_texture() = default;

        color_type get_color(const point3<T>& location, const coord2<T>& coord) const override
        {
            return get_color_at_location(coord);
        }

        virtual color_type get_color_at_location(const coord2<T>& location) const = 0;
    };
}
