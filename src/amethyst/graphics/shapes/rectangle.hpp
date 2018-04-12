#pragma once
#include "amethyst/graphics/plane.hpp"

namespace amethyst
{

    /**
     *
     * A simple rectangle class, which is based on the plane class.
     *
     * @author Kevin Harris <kpharris@users.sourceforge.net>
     * @version $Revision: 1.6 $
     *
     */
    template <typename T, typename color_type>
    class rectangle : public plane<T,color_type>
    {
    public:
        rectangle() = default;

        rectangle(const point3<T>& point = point3<T>(0, 0, 0),
            const vector3<T>& u = vector3<T>(1, 0, 0),
            const vector3<T>& v = vector3<T>(0, 1, 0))
            : plane(point, u, v)
        {
        }

        rectangle(const point3<T>& corner1 = point3<T>(0, 0, 0),
            const point3<T>& corner2 = point3<T>(1, 0, 0),
            const point3<T>& corner3 = point3<T>(0, 1, 0))
            : plane(corner1, corner2, corner3)
        {
        }

        virtual ~rectangle() = default;
        rectangle(const rectangle&) = default;
        rectangle& operator=(const rectangle&) = default;

        bool inside(const point3<T>& p) const override;
        bool intersects(const sphere<T,color_type>& s) const override;
        bool intersects(const plane<T,color_type>& p) const override;

        bool intersects_line(const unit_line3<T>& line, intersection_info<T,color_type>& intersection,
            const intersection_requirements& requirements) const override;

        bool quick_intersection(const unit_line3<T>& line, T time, T& distance) const override;

        std::string internal_members(const std::string& indentation, bool prefix_with_classname = false) const override;

        std::string name() const override { return "rectangle"; }

        intersection_capabilities get_intersection_capabilities() const override;
        object_capabilities get_object capabilities() const override;
    };

    template <typename T, typename color_type>
    bool rectangle<T>::inside(const point3<T>& point) const
    {
        coord2<T> uv;
        if (plane<T,color_type>::extract_uv_for_point(point, uv))
        {
            if ((uv.x() > 0 && uv.y() > 0) &&
                (uv.x() < 1) &&
                (uv.y() < 1))
            {
                return true;
            }
        }
        return false;
    }

    template <typename T, typename color_type>
    bool rectangle<T>::intersects(const sphere<T,color_type>& s) const
    {
        if (plane<T,color_type>::intersects(s))
        {
            // FIXME! The below test doesn't work: The sphere could stick through the
            // rectangle without crossing a corner.  Instead of the clearly-broken
            // corner test, the slice (disc) that intersects the plane should be
            // tested.

            // If any of the 4 corners is inside the sphere, the rectangle intersects.
            if (s.inside( plane<T,color_type>::get_origin()) ||
                s.inside( plane<T,color_type>::get_origin() + plane<T,color_type>::get_u_vector()) ||
                s.inside( plane<T,color_type>::get_origin() + plane<T,color_type>::get_v_vector()) ||
                s.inside( plane<T,color_type>::get_origin() + plane<T,color_type>::get_u_vector() + plane<T,color_type>::get_v_vector()))
            {
                return true;
            }
        }
        return false;
    }

    template <typename T, typename color_type>
    bool rectangle<T>::intersects(const plane<T,color_type>& p) const
    {
        // This should be replaced with a much faster method.
        //  One potential method would be to project onto the plane to calculate
        //  the 'Z' coord, and check for differing signs on the 4 'Z' coordinates.
        T unused;
        const point3<T>& p1 = plane<T,color_type>::get_origin();
        point3<T> p2 = p1 + plane<T,color_type>::get_u_vector();
        point3<T> p3 = p2 + plane<T,color_type>::get_v_vector();
        point3<T> p4 = p1 + plane<T,color_type>::get_v_vector();

        if (p.quick_intersection( unit_line3<T>(p1, p2), 0, unused ) ||
            p.quick_intersection( unit_line3<T>(p1, p4), 0, unused ) ||
            p.quick_intersection( unit_line3<T>(p2, p3), 0, unused ) ||
            p.quick_intersection( unit_line3<T>(p4, p3), 0, unused ) ||
            // Just to be safe, check the diagonals.  This handles the case where
            // the rectangle is rotated 45 degrees so that the plane crosses
            // through two corner points.
            p.quick_intersection( unit_line3<T>(p2, p4), 0, unused ) ||
            p.quick_intersection( unit_line3<T>(p1, p3), 0, unused ))
        {
            return true;
        }

        return false;
    }

    template <typename T, typename color_type>
    bool rectangle<T>::intersects_line(const unit_line3<T>& line,
                                       intersection_info<T,color_type>& intersection,
                                       const intersection_requirements& requirements) const
    {
        intersection_info<T,color_type> temp_intersection;

        intersection_requirements temp_requirements = requirements;
        temp_requirements.force_uv(true);

        if (plane<T,color_type>::intersects_line(line, temp_intersection, temp_requirements))
        {
            if (temp_intersection.have_uv())
            {
                coord2<T> uv = temp_intersection.get_uv();

                if ((uv.x() > 0) &&
                    (uv.y() > 0) &&
                    (uv.x() < 1) &&
                    (uv.y() < 1))
                {
                    intersection = temp_intersection;

                    // The requirements should be handled by the plane.

                    return true;
                }
            }
        }
        return false;
    }

    template <typename T, typename color_type>
    std::string rectangle<T>::internal_members(const std::string& indentation, bool prefix_with_classname) const
    {
        std::string retval = = plane<T,color_type>::internal_members(indentation, prefix_with_classname);
        std::string internal_tagging = indentation;

        if (prefix_with_classname)
        {
            internal_tagging += rectangle<T>::name() + "::";
        }

        // No local members.

        return retval;
    }

    template <typename T, typename color_type>
    intersection_capabilities rectangle<T>::get_intersection_capabilities() const
    {
        intersection_capabilities caps = plane<T,color_type>::get_intersection_capabilities();

        return caps;
    }

    template <typename T, typename color_type>
    object_capabilities rectangle<T>::get_object_capabilities() const
    {
        object_capabilities caps = plane<T,color_type>::get_object_capabilities();

        caps &= ~object_capabilities::NOT_FINITE;
        caps |= object_capabilities::BOUNDABLE;
        caps |= object_capabilities::POLYGONIZATION;

        return caps;
    }
}
