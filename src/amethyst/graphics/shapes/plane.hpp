#pragma once

#include "amethyst/general/string_format.hpp"
#include "amethyst/graphics/shapes/shape.hpp"
#include "amethyst/graphics/shapes/sphere.hpp"
#include "amethyst/math/coord2.hpp"

namespace amethyst
{

    /**
     *
     * A plane in 3d.
     *
     * @author Kevin Harris <kpharris@users.sourceforge.net>
     * @version $Revision: 1.16 $
     *
     */
    template <typename T, typename color_type>
    class plane : public shape<T,color_type>
    {
    public:
        plane() = default;

        // position/normal
        plane(const point3<T>& p, const vector3<T>& n);
        // position/normal/rotation
        plane(const point3<T>& p, const vector3<T>& n, const vector3<T>& vec_u);
        // plane from 3 points.  Must not be colinear.
        plane(const point3<T>& p, const point3<T>& plus_u, const point3<T>& plus_v);
        // Fully-specified
        plane(const point3<T>& p, const vector3<T>& normal, const vector3<T>& vec_u, const vector3<T>& vec_v);

        virtual ~plane() = default;
        plane(const plane&) = default;
        plane& operator=(const plane&) = default;

        const point3<T>& get_origin() const { return defining_point; }
        const vector3<T>& get_normal() const { return normal; }
        const vector3<T>& get_u_vector() const { return u_vector; }
        const vector3<T>& get_v_vector() const { return v_vector; }

        bool inside(const point3<T>& p) const override;
        bool intersects(const sphere<T,color_type>& s) const override;
        bool intersects(const plane<T,color_type>& p) const override;

        bool intersects_line(const unit_line3<T>& line, intersection_info<T,color_type>& intersection,
            const intersection_requirements& requirements) const override;

        /**
         * A quick intersection test.  This will calculate nothing but the
         * distance. This is most useful for shadow tests, and other tests where no
         * textures will be applied.
         */
        virtual bool quick_intersection(const unit_line3<T>& line,
                                        T time, T& distance) const;

        virtual std::string internal_members(const std::string& indentation, bool prefix_with_classname = false) const;

        virtual std::string name() const {
            return "plane";
        }

        // Check to see if the given point is on the plane, and if so, get the UV
        // coordinates of the point.
        bool extract_uv_for_point(const point3<T>& point, coord2<T>& uv) const;

        // This doesn't check to see if the point is on the plane.  As such, it is
        // a speed improvement when the point is already known to be on the plane.
        void extract_uv_for_point_nonchecked(const point3<T>& point, coord2<T>& uv) const;

        virtual intersection_capabilities get_intersection_capabilities() const;
        virtual object_capabilities get_object_capabilities() const;

    private:
        void setup_non_zero_indices();

        point3<T> defining_point = { 0, 0, 0 };
        vector3<T> normal = { 0, 0, 1 };
        vector3<T> v_vector = { 0, 1, 0 };
        vector3<T> u_vector = { 1, 0, 0 };
        int non_zero_u_index = 0;
        int non_zero_v_index = 0;
    };

    template <typename T, typename color_type>
    plane<T,color_type>::plane(const point3<T>& p, const vector3<T>& n)
        : defining_point(p)
        , normal(unit(n))
    {
        calculate_perpendicular_vectors(normal, u_vector, v_vector);

        setup_non_zero_indices();
    }

    template <typename T, typename color_type>
    plane<T,color_type>::plane(const point3<T>& p, const vector3<T>& n, const vector3<T>& vec_u)
        : defining_point(p)
        , normal(unit(n))
        , v_vector(crossprod(normal, vec_u))
        , u_vector(crossprod(v_vector, normal))
    {
        setup_non_zero_indices();
    }

    template <typename T, typename color_type>
    plane<T,color_type>::plane(const point3<T>& p, const point3<T>& plus_u, const point3<T>& plus_v)
        : defining_point(p)
        , v_vector(plus_v - p)
        , u_vector(plus_u - p)
    {
        normal = unit(crossprod(u_vector, v_vector));
        setup_non_zero_indices();
    }

    template <typename T, typename color_type>
    plane<T,color_type>::plane(const point3<T>& p, const vector3<T>& normal, const vector3<T>& vec_u, const vector3<T>& vec_v)
        : defining_point(p)
        , normal(unit(crossprod(vec_u, vec_v)))
        , v_vector(vec_v)
        , u_vector(vec_u)
    {
        setup_non_zero_indices();
    }

    // Returns if the given point is inside the shape.
    template <typename T, typename color_type>
    inline bool plane<T,color_type>::inside(const point3<T>& p) const
    {
        T dist = dotprod(p - defining_point, normal);
        return (dist < AMETHYST_EPSILON) && (dist > -AMETHYST_EPSILON);
    }

    // Returns if the given sphere intersects the shape.
    template <typename T, typename color_type>
    inline bool plane<T,color_type>::intersects(const sphere<T,color_type>& s) const
    {
        T dist = dotprod(s.get_center() - defining_point, normal);
        T max_dist = s.get_radius() + AMETHYST_EPSILON;
        return (dist < max_dist) && (dist > -max_dist);
    }

    // Returns if the given plane intersects the shape.
    template <typename T, typename color_type>
    inline bool plane<T,color_type>::intersects(const plane<T,color_type>& p) const
    {
        T normal_proj = dotprod(normal, p.normal);

        // Flip it if required.
        if (normal_proj < T(0))
        {
            normal_proj = -normal_proj;
        }

        // If they are not parallel, then they intersect somewhere.
        if (normal_proj < (1 - AMETHYST_EPSILON))
        {
            return true;
        }
        // If they are parallel, but have the same defining point, then they
        // overlap EVERYWHERE (the same plane).
        else if (squared_length(defining_point - p.defining_point) < AMETHYST_EPSILON)
        {
            return true;
        }

        return false;
    }


    // Returns if the given line intersects the plane.
    template <typename T, typename color_type>
    inline bool plane<T,color_type>::intersects_line(const unit_line3<T>& line,
        intersection_info<T,color_type>& intersection, const intersection_requirements& requirements) const
    {
        T t;
        T time = 0; // not used
        if (!plane::quick_intersection(line, time, t))
        {
            return false;
        }

        intersection.set_shape(this);
        point3<T> p = line.point_at(t);
        intersection.set_ray(line);
        intersection.set_first_distance(t);
        intersection.set_first_point(p);

        if (requirements.needs_normal())
        {
            intersection.set_normal(normal);
        }

        if (requirements.needs_uv())
        {
            coord2<T> uv;
            // Note that a non-checked version is being called, because the point
            // *SHOULD* be on the plane (it hit it, why check it?).  This improves
            // speed, and reduces artifacts with expanded error (a point that is on
            // the plane would sometimes not show as being on the plane -- this is
            // the case when a small epsilon and single-precision floats are used.
            extract_uv_for_point_nonchecked(p, uv);
            intersection.set_uv(uv);
        }

        if (requirements.needs_local_coord_system())
        {
            intersection.set_onb(onb<T>(u_vector, v_vector, normal));
        }

        return true;
    }

    // Returns if the given line intersects the plane.
    template <typename T, typename color_type>
    inline bool plane<T,color_type>::quick_intersection(const unit_line3<T>& line, T time, T& distance) const
    {
        (void)time;  // This plane doesn't move.
        T ctheta = dotprod(line.direction(), normal);
        T t;

        // I will need to explain why this is safe to do.  If the angle is > 0 or
        // the angle is < 0, then we know there is a hit somewhere, and it can be
        // found.  If the angle is zero, then it will cause a divide by zero,
        // setting t to be +/-INF, which when compared to anything (even itself)
        // returns false, thus, the inside test below can be called with the INF,
        // and not get bad numbers out.
        //
        // DON'T DELETE THESE COMMENTS, AS THEY APPLY TO OTHER PLANE-BASED
        // FUNCTIONS AS WELL!
        if (ctheta > 0)
        {
            t = (dotprod(defining_point - line.origin(), normal) / ctheta);
        }
        else
        {
            t = (dotprod(defining_point - line.origin(), -normal) / -ctheta);
        }

        if (line.inside(t))
        {
            distance = t;
            return true;
        }

        return false;
    }

    template <typename T, typename color_type>
    std::string plane<T,color_type>::internal_members(const std::string& indentation, bool prefix_with_classname) const
    {
        std::string retval;
        std::string internal_tagging = indentation;

        if (prefix_with_classname)
        {
            internal_tagging += plane<T,color_type>::name() + "::";
        }

        retval += indentation + string_format("intersection_capabilities=%1\n", inspect(get_intersection_capabilities()));
        retval += indentation + string_format("object_capabilities=%1\n", inspect(get_object_capabilities()));
        retval += internal_tagging + string_format("point=%1\n", defining_point);
        retval += internal_tagging + string_format("normal=%1\n", normal);
        retval += internal_tagging + string_format("u=%1\n", u_vector);
        retval += internal_tagging + string_format("v=%1\n", v_vector);
        retval += internal_tagging + string_format("nz_indices=(%1,%2)\n",
                                                   non_zero_u_index,
                                                   non_zero_v_index);
        return retval;
    }

    template <typename T, typename color_type>
    void plane<T,color_type>::setup_non_zero_indices()
    {
        for (non_zero_u_index = 0; non_zero_u_index < 3; ++non_zero_u_index)
        {
            if (fabs(u_vector[non_zero_u_index]) > AMETHYST_EPSILON) {
                break;
            }
        }
        for (non_zero_v_index = 0; non_zero_v_index < 3; ++non_zero_v_index)
        {
            if (non_zero_v_index == non_zero_u_index) { continue; }
            if (fabs(v_vector[non_zero_v_index]) > AMETHYST_EPSILON) {
                break;
            }
        }
    }

    template <typename T, typename color_type>
    inline bool plane<T,color_type>::extract_uv_for_point(const point3<T>& point,
                                               coord2<T>& uv) const
    {
        if (plane<T,color_type>::inside(point))
        {
            T u;
            T v;
            vector3<T> point_diff_vector(point - defining_point);
            T u_scalar = u_vector[non_zero_v_index] / u_vector[non_zero_u_index];
            v = ((point_diff_vector[non_zero_v_index] -
                  point_diff_vector[non_zero_u_index] * u_scalar) /
                 (v_vector[non_zero_v_index] -
                  v_vector[non_zero_u_index] * u_scalar));
            u = ((point_diff_vector[non_zero_u_index] -
                  v * v_vector[non_zero_u_index]) /
                 u_vector[non_zero_u_index]);

            uv.set(u, v);

            return true;
        }
        return false;
    }


    template <typename T, typename color_type>
    inline void plane<T,color_type>::extract_uv_for_point_nonchecked(const point3<T>& point,
                                                          coord2<T>& uv) const
    {
        T u;
        T v;
        vector3<T> point_diff_vector(point - defining_point);
        T u_scalar = u_vector[non_zero_v_index] / u_vector[non_zero_u_index];
        v = ((point_diff_vector[non_zero_v_index] -
              point_diff_vector[non_zero_u_index] * u_scalar) /
             (v_vector[non_zero_v_index] -
              v_vector[non_zero_u_index] * u_scalar));
        u = ((point_diff_vector[non_zero_u_index] -
              v * v_vector[non_zero_u_index]) /
             u_vector[non_zero_u_index]);

        uv.set(u, v);
    }


    template <typename T, typename color_type>
    bool intersects(const sphere<T,color_type>& s, const plane<T,color_type>& p)
    {
        return p.intersects(s);
    }


    template <typename T, typename color_type>
    intersection_capabilities plane<T,color_type>::get_intersection_capabilities() const
    {
        intersection_capabilities caps = shape<T,color_type>::get_intersection_capabilities();

        caps |= intersection_capabilities::HIT_FIRST;
        caps |= intersection_capabilities::NORMAL_CALCULATION;
        caps |= intersection_capabilities::UV_CALCULATION;
        caps |= intersection_capabilities::LOCAL_SYSTEM_CALCULATION;
        return caps;
    }

    template <typename T, typename color_type>
    object_capabilities plane<T,color_type>::get_object_capabilities() const
    {
        object_capabilities caps = shape<T,color_type>::get_object_capabilities();

        caps |= object_capabilities::NOT_FINITE;
        caps |= object_capabilities::SIMPLE;

        return caps;
    }
}
