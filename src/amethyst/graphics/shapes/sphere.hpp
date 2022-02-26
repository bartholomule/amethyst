#pragma once

#include "amethyst/graphics/shapes/shape.hpp"
#include "amethyst/general/defines.hpp"
#include "amethyst/general/string_format.hpp"


namespace amethyst
{
    // Find the first positive T distance along the supplied line which intersects a
    // sphere of the given center and radius.
    template <typename T>
    bool quick_sphere_intersection_test(const point3<T>& center, T radius, T radius_squared,
        const unit_line3<T>& line, T& distance)
    {
        const vector3<T> o_c = line.origin() - center;
        const T A = dotprod(line.direction(), line.direction());
        const T B = 2 * dotprod(line.direction(), o_c);
        const T C = dotprod(o_c, o_c) - radius_squared;
        const T discriminant = B * B - 4 * A * C;

        if (discriminant < 0)
        {
            return false;
        }

        const T sqrtd = sqrt(discriminant);

        // If t1 is inside, it MUST be the closest, as A will always be positive
        // (squared length of the line), and the subtraction will be less than
        // the addition (as the square root will always be positive).
        const T t1 = (-B - sqrtd) / (2 * A);
        if (line.inside(t1))
        {
            distance = t1;
            return true;
        }

        // The first side (although a hit), wasn't inside the range.
        const T t2 = (-B + sqrtd) / (2 * A);
        if (line.inside(t2))
        {
            distance = t2;
            return true;
        }
        return false;
    }


    /**
     *
     * A sphere class.
     *
     * @author Kevin Harris <kpharris@users.sourceforge.net>
     * @version $Revision: 1.10 $
     *
     */
    template <typename T, typename color_type>
    class sphere : public shape<T, color_type>
    {
    public:
        using parent = shape<T, color_type>;

        sphere() = default;
        sphere(const sphere& old) = default;
        sphere(const point3<T>& c, T rad, texture_ptr<T,color_type> tex = nullptr)
            : shape<T,color_type>(tex)
            , m_center(c)
            , m_radius(rad)
            , m_radius_squared(rad * rad)
        {
            if (m_radius < T(0))
            {
                m_radius = -m_radius;
            }
        }
        virtual ~sphere() = default;
        sphere& operator=(const sphere& old) = default;

        point3<T> get_center() const { return m_center; }
        T get_radius() const { return m_radius; }

        bool inside(const point3<T>& p) const override;
        bool intersects(const sphere<T,color_type>& s) const override;
        bool intersects(const plane<T,color_type>& p) const override;

        using shape<T,color_type>::intersects_line;
        bool intersects_line(const unit_line3<T>& line,
            intersection_info<T,color_type>& intersection, const intersection_requirements& requirements) const override;

        bool quick_intersection(const unit_line3<T>& line, T time, T& distance) const override;

        std::string internal_members(const std::string& indentation, bool prefix_with_classname = false) const override;

        std::string name() const override { return "sphere"; }

        intersection_capabilities get_intersection_capabilities() const override;
        object_capabilities get_object_capabilities() const override;
    private:
        coord2<T> get_uv(const point3<T>& location) const;

        point3<T> m_center = { 0, 0, 0 };
        T m_radius = 0;
        T m_radius_squared = 0;
    };

    // Returns if the given point is inside the sphere.
    template <typename T, typename color_type>
    bool sphere<T,color_type>::inside(const point3<T>& p) const
    {
        // The epsilon adjusted radius is (r^2 + 2*r*E + E^2)
        return squared_length(p - m_center) <
               (m_radius_squared +
                2 * m_radius * AMETHYST_EPSILON +
                AMETHYST_EPSILON * AMETHYST_EPSILON);
    }

    // Returns if the given sphere intersects the sphere.
    template <typename T, typename color_type>
    bool sphere<T,color_type>::intersects(const sphere<T,color_type>& s) const
    {
        T combined_radius = s.m_radius + m_radius + AMETHYST_EPSILON;
        return squared_length(s.m_center - m_center) <
               (combined_radius * combined_radius);
    }

    // Returns if the given plane intersects the shape.
    template <typename T, typename color_type>
    bool sphere<T,color_type>::intersects(const plane<T,color_type>& p) const
    {
        // FIXME! Extract the sphere<->plane intersection.
        return false;
    }



    // Returns if the given line intersects the sphere.
    template <typename T, typename color_type>
    bool sphere<T,color_type>::intersects_line(const unit_line3<T>& line,
                                    intersection_info<T,color_type>& intersection,
                                    const intersection_requirements& requirements) const
    {
        // FIXME! Find both intersections if required.
        T distance;
        if (quick_sphere_intersection_test(m_center, m_radius, m_radius_squared, line, distance))
        {
            intersection.set_shape(this);
            intersection.set_first_distance(distance);
            intersection.set_first_point(line.point_at(distance));
            intersection.set_ray(line);
            if (requirements.needs_normal())
            {
                intersection.set_normal(unit(intersection.get_first_point() - m_center));
            }

            // FIXME! Follow the requirements
            if (requirements.needs_uv())
            {
                intersection.set_uv(get_uv(intersection.get_first_point()));
            }

            return true;
        }
        return false;
    }

    /**
     * A quick intersection test.  This will calculate nothing but the
     * distance. This is most useful for shadow tests, and other tests where no
     * textures will be applied.
     */
    template <typename T, typename color_type>
    bool sphere<T,color_type>::quick_intersection(const unit_line3<T>& line,
                                       T time, T& distance) const
    {
        (void)time;  // not a moving or resizing sphere...
        return quick_sphere_intersection_test(m_center, m_radius, m_radius_squared, line, distance);
    }

    template <typename T, typename color_type>
    std::string sphere<T,color_type>::internal_members(const std::string& indentation, bool prefix_with_classname) const
    {
        std::string retval;
        std::string internal_tagging = indentation;

        if (prefix_with_classname)
        {
            internal_tagging += sphere<T,color_type>::name() + "::";
        }

        retval += indentation + string_format("intersection_capabilities=%1\n", amethyst::to_string(get_intersection_capabilities()));
        retval += indentation + string_format("object_capabilities=%1\n", amethyst::to_string(get_object_capabilities()));
        retval += internal_tagging + string_format("center=%1\n", m_center);
        retval += internal_tagging + string_format("radius=%1\n", m_radius);
        retval += internal_tagging + string_format("texture=%1\n", parent::m_texture);

        return retval;
    }

    template <typename T, typename color_type>
    intersection_capabilities sphere<T,color_type>::get_intersection_capabilities() const
    {
        intersection_capabilities caps = shape<T,color_type>::get_intersection_capabilities();

        caps |= intersection_capabilities::HIT_FIRST;
        caps |= intersection_capabilities::HIT_ALL;
        caps |= intersection_capabilities::NORMAL_CALCULATION;
        caps |= intersection_capabilities::UV_CALCULATION;
        //    caps |= intersection_capabilities::LOCAL_SYSTEM_CALCULATION
        return caps;
    }

    template <typename T, typename color_type>
    object_capabilities sphere<T,color_type>::get_object_capabilities() const
    {
        object_capabilities caps = shape<T,color_type>::get_object_capabilities();

        caps |= object_capabilities::BOUNDABLE;
        caps |= object_capabilities::SIMPLE;

        return caps;
    }

    template <typename T, typename color_type>
    coord2<T> sphere<T,color_type>::get_uv(const point3<T>& location) const
    {
        constexpr T twopi = 2 * M_PI;
        constexpr T inv_twopi = 1.0 / twopi;
        constexpr T inv_pi = 1.0 / M_PI;

        vector3<T> point_vector = (location - m_center) / m_radius;

        T cos_theta = point_vector.y();
        // phi = [-pi,pi]
        // theta = [0,pi]
        T theta = acos(cos_theta);
        T phi = atan2(point_vector.z(), point_vector.x());

        T u = (M_PI - phi) * inv_twopi;
        T v = 1 - theta * inv_pi;

        return { u, v };
    }
}