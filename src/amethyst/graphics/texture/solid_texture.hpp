#pragma once

#include "amethyst/graphics/texture/texture.hpp"

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

        color_type get_color(const point3<T>& location, const coord2<T>& coord, const vector3<T>& normal) const override
        {
            return get_color_at_point(location, normal);
        }

        virtual color_type get_color_at_point(const point3<T>& location, const vector3<T>& normal) const = 0;

        std::string name() const override
        {
            return "solid_texture";
        }

        material_capabilities get_material_capabilties() const override
        {
            return material_capabilities::DIFFUSE;
        }

    };
}
