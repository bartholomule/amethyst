#pragma once

#include "amethyst/math/point3.hpp"
#include "amethyst/math/coord2.hpp"
#include "amethyst/general/string_dumpable.hpp"
#include "amethyst/graphics/ray_parameters.hpp"
#include "amethyst/graphics/capabilities.hpp"

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

        virtual bool get_color(const point3<T>& location, const coord2<T>& coord, const vector3<T>& normal, color_type& color) const
        {
            return false;
        }

        color_type get_color(const point3<T>& location, const coord2<T>& coord, const vector3<T>& normal) const
        {
            color_type result;
            if (!get_color(location, coord, normal, result))
            {
                result = { 0, 0, 0 };
            }
            return result;
        }

        virtual bool scatter_ray(const ray_parameters<T,color_type>& ray, const intersection_info<T,color_type>& intersection, ray_parameters<T,color_type>& reflected, color_type& attenuation) const
        {
            return false;
        }

        std::string name() const override
        {
            return "texture";
        }

        virtual material_capabilities get_material_capabilties() const = 0;
    };

    template <typename T, typename color_type>
    using texture_ptr = std::shared_ptr<texture<T, color_type>>;
}