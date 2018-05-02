#pragma once

#include "amethyst/graphics/texture/texture.hpp"

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

        using texture<T, color_type>::get_color;
        bool get_color(const point3<T>& location, const coord2<T>& coord, const vector3<T>& normal, color_type& color) const override
        {
            color = get_color_at_location(coord, normal);
            return true;
        }

        virtual color_type get_color_at_location(const coord2<T>& location, const vector3<T>& normal) const = 0;


        std::string name() const override
        {
            return "surface_texture";
        }

        material_capabilities get_material_capabilties() const override
        {
            return material_capabilities::DIFFUSE;
        }
    };
}
