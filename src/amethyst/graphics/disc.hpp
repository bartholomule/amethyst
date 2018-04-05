#pragma once

#include "amethyst/graphics/plane.hpp"
#include "amethyst/general/string_format.hpp"
#include "amethyst/math/template_math.hpp"

namespace amethyst
{
    /**
     *
     * A disc (circle).
     *
     * @author Kevin Harris <kpharris@users.sourceforge.net>
     * @version $Revision: 1.3 $
     *
     */
    template <class T>
    class disc : public plane<T>
    {
    public:
        disc() = default;
        disc(const point3<T>& point, T radius = 1, const vector3<T>& normal = vector3<T>(0, 0, 1))
            : plane<T>(point, normal)
            , m_radius(radius)
            , m_radius_squared(radius * radius)
        {
        }

        disc(const point3<T>& point, T radius, const vector3<T>& normal, const vector3<T>& u)
            : plane<T>(point, normal, unit(u))
            , m_radius(radius)
            , m_radius_squared(radius * radius)
        {
        }

        disc(const disc& old) = default;
        virtual ~disc() = default;
        disc& operator=(const disc& old) = default;

        /** Returns if the given point is inside the shape. */
        bool inside(const point3<T>& p) const override;

        /** Returns if the given sphere intersects the shape. */
        bool intersects(const sphere<T>& s) const override;

        /** Returns if the given plane intersects the shape. */
        bool intersects(const plane<T>& p) const override;

        /** Returns if the given line intersects the plane. */
        bool intersects_line(const unit_line3<T>& line,
            intersection_info<T>& intersection,
            const intersection_requirements& requirements) const override;

        /**
         * A quick intersection test.  This will calculate nothing but the
         * distance. This is most useful for shadow tests, and other tests where no
         * textures will be applied.
         */
        bool quick_intersection(const unit_line3<T>& line,
            T time, T& distance) const override;

        std::string internal_members(const std::string& indentation, bool prefix_with_classname = false) const override;

        std::string name() const override { return "disc"; }

        intersection_capabilities get_intersection_capabilities() const override;
        object_capabilities get_object_capabilities() const override;

    private:
        T m_radius = T(1);
        T m_radius_squared = T(1);
    };

    template <class T>
    bool disc<T>::inside(const point3<T>& point) const
    {
        coord2<T> uv;
        if (plane<T>::extract_uv_for_point(point, uv))
        {
            return ((uv.x() * uv.x() + uv.y() * uv.y()) < m_radius_squared);
        }
        return false;
    }

    template <class T>
    bool disc<T>::intersects(const sphere<T>& s) const
    {
        vector3<T> sphere_vector = s.get_center() - plane<T>::get_origin();
        T projected_n = dotprod(sphere_vector, plane<T>::get_normal());
        T projected_n_squared = projected_n * projected_n;
        T sphere_radius_squared = s.get_radius() * s.get_radius();

        if (sphere_radius_squared >= projected_n_squared)
        {
            // We know that the distance from the center of the sphere to the edge
            // of the projected disc is the sphere's radius.
            //
            // We know the distance from the center of the sphere to the plane is
            // abs(n dot (s.c - o))
            //
            // Using the Pythagorean theorem, the projected radius is:
            T projected_radius = sqrt(sphere_radius_squared - projected_n_squared);

            // To intersect our disc, the projected disc's radius plus our radius
            // must be less than the distance to our projected center.
            T projected_u = dotprod(sphere_vector, plane<T>::get_u_vector());
            T projected_v = dotprod(sphere_vector, plane<T>::get_v_vector());

            T distance_to_projected_center = sqrt(projected_u * projected_u + projected_v * projected_v);

            // Use epsilon to determine if it is close enough to consider an
            // intersection.
            if ((m_radius + projected_radius + AMETHYST_EPSILON) > distance_to_projected_center)
            {
                return true;
            }
        }

        return false;
    }

    template <class T>
    bool disc<T>::intersects(const plane<T>& p) const
    {
        // FIXME!

        return false;
    }

    template <class T>
    bool disc<T>::intersects_line(const unit_line3<T>& line,
                                  intersection_info<T>& intersection,
                                  const intersection_requirements& requirements) const
    {
        intersection_info<T> temp_intersection;

        intersection_requirements temp_requirements = requirements;
        temp_requirements.force_uv(true);

        if (plane<T>::intersects_line(line, temp_intersection, temp_requirements))
        {
            if (temp_intersection.have_uv())
            {
                coord2<T> uv = temp_intersection.get_uv();

                if ((uv.x() * uv.x() + uv.y() * uv.y()) < (m_radius_squared + AMETHYST_EPSILON))
                {
                    intersection = temp_intersection;

                    // The requirements should be handled by the plane.

                    return true;
                }
            }
        }
        return false;
    }

    // Returns if the given line intersects the plane.
    template <class T>
    inline bool disc<T>::quick_intersection(const unit_line3<T>& line,
                                            T time, T& distance) const
    {
        T plane_dist;
        if (plane<T>::quick_intersection(line, time, plane_dist))
        {
            vector3<T> inter_v = plane<T>::get_origin() - (line.origin() + plane_dist * line.direction());
            if (dotprod(inter_v, inter_v) < (m_radius_squared + AMETHYST_EPSILON))
            {
                distance = plane_dist;
                return true;
            }
        }

        return false;
    }

    template <class T>
    std::string disc<T>::internal_members(const std::string& indentation, bool prefix_with_classname) const
    {
        std::string retval = plane<T>::internal_members(indentation, true);
        std::string internal_tagging = indentation;

        if (prefix_with_classname)
        {
            internal_tagging += disc<T>::name() + "::";
        }

        retval += internal_tagging + string_format("radius=%1\n", m_radius);

        return retval;
    }

    template <class T>
    intersection_capabilities disc<T>::get_intersection_capabilities() const
    {
        intersection_capabilities caps = plane<T>::get_intersection_capabilities();

        return caps;
    }

    template <class T>
    object_capabilities disc<T>::get_object_capabilities() const
    {
        object_capabilities caps = plane<T>::get_object_capabilities();

        caps &= ~object_capabilities::NOT_FINITE;
        caps |= object_capabilities::BOUNDABLE;
        caps |= object_capabilities::POLYGONIZATION;

        return caps;
    }
}
