#pragma once

#include "amethyst/graphics/shapes/shape_fwd.hpp"
#include "amethyst/math/point3.hpp"
#include "amethyst/math/line3.hpp"
#include "amethyst/math/unit_line3.hpp"
#include "amethyst/graphics/intersection_info.hpp"
#include "amethyst/graphics/capabilities.hpp"
#include "amethyst/graphics/requirements.hpp"
#include "amethyst/graphics/ray_parameters.hpp"
#include "amethyst/general/string_dumpable.hpp"
#include "amethyst/graphics/texture/texture.hpp"

namespace amethyst
{
    /**
     *
     * The base class for a shape.
     *
     * @author Kevin Harris <kpharris@users.sourceforge.net>
     * @version $Revision: 1.12 $
     *
     */
    template <typename T, typename color_type>
    class shape : public string_dumpable
    {
    public:
        using base_type = T;

        shape() = default;
        shape(texture_ptr<T, color_type> tex) : m_texture(tex) { }
        virtual ~shape() = default;
        shape(const shape& old) = default;
        shape& operator= (const shape& old) = default;

        /** Returns if the given point is inside the shape. */
        virtual bool inside(const point3<T>& p) const = 0;

        /** Returns if the given sphere intersects the shape. */
        virtual bool intersects(const sphere<T,color_type>& s) const = 0;

        /** Returns if the given plane intersects the shape. */
        virtual bool intersects(const plane<T,color_type>& p) const = 0;

        /**
         * Returns if the given line intersects the shape.  For performance
         * reasons, this may be overridden, as the default just converts the line
         * to a unit line, calls the unit version, and adjusts the distance for the
         * scaling.
         */
        virtual bool intersects_line(const line3<T>& line,
            intersection_info<T,color_type>& intersection,
            const intersection_requirements& requirements = intersection_requirements()) const;

        /** Returns if the given line intersects the shape. */
        virtual bool intersects_line(const unit_line3<T>& line,
            intersection_info<T,color_type>& intersection,
            const intersection_requirements& requirements = intersection_requirements()) const = 0;

        /**
         * Returns if the given ray intersects the shape.
         *
         * The default behavior of this function is to ignore all ray parameters
         * except for the line.  Anything that needs those must override this
         * function.
         *
         * NOTE: This is the function that you will want to use most of the time,
         * since subclasses may act on other properties from the ray (eg. movable
         * objects).
         *
         */
        virtual bool intersects_ray(const ray_parameters<T,color_type>& ray,
            intersection_info<T,color_type>& intersection,
            const intersection_requirements& requirements = intersection_requirements()) const;

        /**
         * A quick intersection test.  This will calculate nothing but the
         * distance. This is most useful for shadow tests, and other tests where no
         * textures will be applied.
         */
        virtual bool quick_intersection(const unit_line3<T>& line, T time, T& distance) const = 0;

        virtual std::string name() const
        {
            return "shape";
        }

        virtual intersection_capabilities get_intersection_capabilities() const
        {
            return intersection_capabilities::NONE;
        }

        virtual object_capabilities get_object_capabilities() const
        {
            return object_capabilities::NONE;
        }

        texture_ptr<T, color_type> texture() const
        {
            return m_texture;
        }

    protected:
        texture_ptr<T, color_type> m_texture;
    };


    template <typename T, typename color_type>
    bool shape<T, color_type>::intersects_line(const line3<T>& line,
        intersection_info<T,color_type>& intersection,
        const intersection_requirements& requirements) const
    {
        unit_line3<T> ul(line);

        if (intersects_line(ul, intersection, requirements))
        {
            intersection.set_first_distance(intersection.get_first_distance() /
                ul.normal_length());
            return true;
        }
        return false;
    }


    template <typename T, typename color_type>
    bool shape<T, color_type>::intersects_ray(const ray_parameters<T,color_type>& ray,
        intersection_info<T,color_type>& intersection,
        const intersection_requirements& requirements) const
    {
        if (intersects_line(ray.get_line(), intersection, requirements))
        {
            return true;
        }
        return false;
    }
}