#pragma once

#include "amethyst/graphics/shapes/plane.hpp"

namespace amethyst
{
    /**
     *
     * A simple triangle class, which is based on the plane class.
     *
     * @author Kevin Harris <kpharris@users.sourceforge.net>
     * @version $Revision: 1.10 $
     *
     */
    template <typename T, typename color_type>
    class triangle : public plane<T,color_type>
    {
    public:
        triangle() = default;

        triangle(const point3<T>& corner1 = point3<T>(0, 0, 0),
            const point3<T>& corner2 = point3<T>(1, 0, 0),
            const point3<T>& corner3 = point3<T>(0, 1, 0))
            : plane<T,color_type>(corner1, corner2, corner3)
        {
        }
        virtual ~triangle() = default;
        triangle(const triangle&) = default;
        triangle& operator=(const triangle&) = default;

        bool inside(const point3<T>& p) const override;
        bool intersects(const sphere<T,color_type>& s) const override;
        bool intersects(const plane<T,color_type>& p) const override;
        bool intersects_line(const unit_line3<T>& line, intersection_info<T,color_type>& intersection,
            const intersection_requirements& requirements) const override;
        bool quick_intersection(const unit_line3<T>& line, T time, T& distance) const override;

        std::string internal_members(const std::string& indentation, bool prefix_with_classname = false) const override;
        std::string name() const override { return "triangle"; }

        intersection_capabilities get_intersection_capabilities() const override;
        object_capabilities get_object_capabilities() const override;
    };


    template <typename T, typename color_type>
    bool triangle<T,color_type>::inside(const point3<T>& point) const
    {
        coord2<T> uv;
        if (plane<T,color_type>::extract_uv_for_point(point, uv))
        {
            if ((uv.x() > 0 && uv.y() > 0) &&
                (uv.x() + uv.y() < 1))
            {
                return true;
            }
        }
        return false;
    }

    template <typename T, typename color_type>
    bool triangle<T,color_type>::intersects(const sphere<T,color_type>& s) const
    {
        // If any of the 3 corners is inside the sphere, the triangle intersects.
        if (s.inside( plane<T,color_type>::get_origin()) ||
            s.inside( plane<T,color_type>::get_origin() + plane<T,color_type>::get_u_vector()) ||
            s.inside( plane<T,color_type>::get_origin() + plane<T,color_type>::get_v_vector()))
        {
            return true;
        }
        return false;
    }

    template <typename T, typename color_type>
    bool triangle<T,color_type>::intersects(const plane<T,color_type>& p) const
    {
        T unused;
        const point3<T>& pp = plane<T,color_type>::get_origin();
        point3<T> pu = plane<T,color_type>::get_origin() + plane<T,color_type>::get_u_vector();
        point3<T> pv = plane<T,color_type>::get_origin() + plane<T,color_type>::get_v_vector();

        if (p.quick_intersection( unit_line3<T>(pp, pu), 0, unused ) ||
            p.quick_intersection( unit_line3<T>(pp, pv), 0, unused ) ||
            p.quick_intersection( unit_line3<T>(pu, pv), 0, unused ))
        {
            return true;
        }

        return false;
    }

    template <typename T, typename color_type>
    bool triangle<T,color_type>::intersects_line(const unit_line3<T>& line, intersection_info<T,color_type>& intersection,
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

                if ((uv.x() > 0 && uv.y() > 0) &&
                    (uv.x() + uv.y() < 1))
                {
                    intersection = temp_intersection;

                    // The requirements should be handled by the plane.

                    return true;
                }
            }
            else
            {
                std::cout << "ERROR! We should have a UV, but none was provided!" << std::endl;
            }
        }
        return false;
    }

    template <typename T, typename color_type>
    std::string triangle<T,color_type>::internal_members(const std::string& indentation, bool prefix_with_classname) const
    {
        std::string retval = plane<T,color_type>::internal_members(indentation, prefix_with_classname);
        std::string internal_tagging = indentation;

        if (prefix_with_classname)
        {
            internal_tagging += triangle<T,color_type>::name() + "::";
        }

        return retval;
    }

    template <typename T, typename color_type>
    intersection_capabilities triangle<T,color_type>::get_intersection_capabilities() const
    {
        intersection_capabilities caps = plane<T,color_type>::get_intersection_capabilities();

        return caps;
    }

    template <typename T, typename color_type>
    object_capabilities triangle<T,color_type>::get_object_capabilities() const
    {
        object_capabilities caps = plane<T,color_type>::get_object_capabilities();

        caps &= ~object_capabilities::NOT_FINITE;
        caps |= object_capabilities::BOUNDABLE;
        caps |= object_capabilities::POLYGONIZATION;

        return caps;
    }

    template <typename T, typename color_type>
    bool triangle<T,color_type>::quick_intersection(const unit_line3<T>& line, T time, T& distance) const
    {
        T temp_distance;

        if (plane<T,color_type>::quick_intersection(line, time, temp_distance))
        {
            point3<T> p = (temp_distance * line.direction() + line.origin());
            coord2<T> uv;

            // Note that a non-checked version is being called, because the point
            // *SHOULD* be on the plane (it hit it, why check it?).  This improves
            // speed, and reduces artifacts with expanded error (a point that is on
            // the plane would sometimes not show as being on the plane -- this is
            // the case when a small epsilon and single-precision floats are used.
            plane<T,color_type>::extract_uv_for_point_nonchecked(p, uv);

            if ((uv.x() > 0 && uv.y() > 0) &&
                (uv.x() + uv.y() < 1))
            {
                distance = temp_distance;
                return true;
            }
        }
        return false;
    }
}
