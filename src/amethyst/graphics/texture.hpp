#pragma once

#include "amethyst/math/point3.hpp"
#include "amethyst/math/coord2.hpp"
#include "amethyst/general/string_dumpable.hpp"

namespace amethyst
{

    /**
     *
     * The base class for a texture.  A texture in itself is not useful for
     * shading an object.  A material is used to assign textures for objects.
     *
     * @author Kevin Harris <kpharris@users.sourceforge.net>
     * @version $Revision: 1.1 $
     *
     */
    template <typename T, typename color_type>
    class texture : public string_dumpable
    {
    public:
        texture() = default;
        virtual ~texture() = default;

        virtual color_type get_color(const point3<T>& location, const coord2<T>& coord, const vector3<T>& normal) const = 0;

        std::string name() const override
        {
            return "texture";
        }
    };
}