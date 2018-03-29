/*
 * $Id: disc.hpp,v 1.3 2008/06/21 22:25:10 kpharris Exp $
 *
 * Part of "Amethyst" -- A playground for graphics development.
 * Copyright (C) 2008 Kevin Harris
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
 */

#if       !defined(AMETHYST__DISC_HPP)
#define            AMETHYST__DISC_HPP

// --------------------------------------
// Default include of parent class header
// --------------------------------------
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

    private:
        T radius;
        T radius_squared;

    protected:

    public:
        disc(const point3<T>& point,
             const T& radius = 1,
             const vector3<T>& normal = vector3<T>(0, 0, 1));

        disc(const point3<T>& point,
             const T& radius,
             const vector3<T>& normal,
             const vector3<T>& u);

        virtual ~disc();

        disc(const disc& old);

        disc& operator= (const disc& old);

        /** Returns if the given point is inside the shape. */
        virtual bool inside(const point3<T>& p) const;

        /** Returns if the given sphere intersects the shape. */
        virtual bool intersects(const sphere<T>& s) const;

        /** Returns if the given plane intersects the shape. */
        virtual bool intersects(const plane<T>& p) const;

        /** Returns if the given line intersects the plane. */
        virtual bool intersects_line(const unit_line3<T>& line,
                                     intersection_info<T>& intersection,
                                     const intersection_requirements& requirements) const;

        /**
         * A quick intersection test.  This will calculate nothing but the
         * distance. This is most useful for shadow tests, and other tests where no
         * textures will be applied.
         */
        virtual bool quick_intersection(const unit_line3<T>& line,
                                        T time, T& distance) const;

        virtual std::string internal_members(const std::string& indentation, bool prefix_with_classname = false) const;

        virtual std::string name() const {
            return "disc";
        }

        virtual intersection_capabilities get_intersection_capabilities() const;
        virtual object_capabilities get_object_capabilities() const;

    }; // class disc


    template <class T>
    disc<T>::disc(const point3<T>& point,
                  const T& radius,
                  const vector3<T>& normal)
        : plane<T>(point, normal)
        , radius(radius)
        , radius_squared(radius * radius)
    {
    }

    template <class T>
    disc<T>::disc(const point3<T>& point,
                  const T& radius,
                  const vector3<T>& normal,
                  const vector3<T>& u)
        : plane<T>(point, normal, unit(u))
        , radius(radius)
        , radius_squared(radius * radius)
    {
    }

    template <class T>
    disc<T>::~disc()
    {
    } // ~disc()

    //--------------------------------
    // Copy constructor for class disc
    //--------------------------------
    template <class T>
    disc<T>::disc(const disc<T>& old) :
        plane<T>(old)
    {
    } // disc(disc)

    template <class T>
    disc<T>& disc<T>::operator= (const disc<T>& old)
    {
        if (&old != this)
        {
            plane<T>::operator=(old);
            radius_squared = old.radius_squared;
        }
        return *this;
    } // disc::operator=(disc)

    template <class T>
    bool disc<T>::inside(const point3<T>& point) const
    {
        coord2<T> uv;
        if (plane<T>::extract_uv_for_point(point, uv))
        {
            if ((uv.x() * uv.x() + uv.y() * uv.y()) < radius_squared)
            {
                return true;
            }
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
            if ((radius + projected_radius + AMETHYST_EPSILON) > distance_to_projected_center)
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

                if ((uv.x() * uv.x() + uv.y() * uv.y()) < (radius_squared + AMETHYST_EPSILON))
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
            if (dotprod(inter_v, inter_v) < (radius_squared + AMETHYST_EPSILON))
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

        retval += internal_tagging + string_format("radius=%1\n", radius);

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

        caps &= ~object_capabilities::INFINITE;
        caps |= object_capabilities::BOUNDABLE;
        caps |= object_capabilities::POLYGONIZATION;

        return caps;
    }

} // namespace amethyst


#endif /* !defined(AMETHYST__DISC_HPP) */

