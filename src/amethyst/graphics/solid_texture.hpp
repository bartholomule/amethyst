#pragma once

#include "amethyst/graphics/texture.hpp"

namespace amethyst
{

    /**
     *
     * A class for a solid texture.  These textures generate colors based on 3d
     * positioning.
     *
     * @author Kevin Harris <kpharris@users.sourceforge.net>
     * @version $Revision: 1.2 $
     *
     */
    template <class T, class color_type>
    class solid_texture : public texture<T, color_type>
    {

    public:
        solid_texture() = default;
        virtual ~solid_texture() = default;

        color_type get_color(const point3<T>& location, const coord2<T>& coord) const override
        {
            return get_color_at_point(location);
        }

        virtual color_type get_color_at_point(const point3<T>& location) const = 0;

        virtual std::string name() const
        {
            return "texture";
        }
    };
}
