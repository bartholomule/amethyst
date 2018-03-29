/*
 * $Id: plane.hpp,v 1.16 2008/06/21 22:25:10 kpharris Exp $
 *
 * Part of "Amethyst" a playground for graphics development
 * Copyright (C) 2004 Kevin Harris
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

#if       !defined(AMETHYST__PLANE_HPP)
#define            AMETHYST__PLANE_HPP

// --------------------------------------
// Default include of parent class header
// --------------------------------------
#include "amethyst/general/string_format.hpp"
#include "amethyst/graphics/shape.hpp"
#include "amethyst/graphics/sphere.hpp"
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
    template <class T>
    class plane : public shape<T>
    {

    private:

        point3<T> defining_point;
        vector3<T> normal;
        vector3<T> u_vector;
        vector3<T> v_vector;
        int non_zero_u_index;
        int non_zero_v_index;

    protected:

    public:
        /** Default constructor */
        plane();

        /** Secondary (pos/orientation) constructor */
        plane(const point3<T>& p, const vector3<T>& n);

        /** Tertiary (pos/orientation/rotation) constructor */
        plane(const point3<T>& p, const vector3<T>& n, const vector3<T>& vec_u);

        /** Quaternary (pos1, pos2, pos3) constructor */
        plane(const point3<T>& p, const point3<T>& plus_u, const point3<T>& plus_v);
        /** Quinary (pos, vec, vec, vec) constructor */
        plane(const point3<T>& p, const vector3<T>& normal, const vector3<T>& vec_u, const vector3<T>& vec_v);

        /** Destructor */
        virtual ~plane();

        /** Copy constructor */
        plane(const plane& old);

        /** Assignment operator */
        plane& operator= (const plane& old);

        const point3<T>& get_origin() const
        {
            return defining_point;
        }
        const vector3<T>& get_normal() const
        {
            return normal;
        }
        const vector3<T>& get_u_vector() const
        {
            return u_vector;
        }
        const vector3<T>& get_v_vector() const
        {
            return v_vector;
        }

        /** Returns if the given point is inside the shape. */
        virtual bool inside(const point3<T>& p) const;

        /** Returns if the given sphere intersects the shape. */
        virtual bool intersects(const sphere<T>& s) const;

        /** Returns if the given plane intersects the shape. */
        virtual bool intersects(const plane<T>& p) const;

        /** Returns if the given line intersects the sphere. */
        virtual bool intersects_line(const line3<T>& line,
                                     intersection_info<T>& intersection,
                                     const intersection_requirements& requirements) const;

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

    }; // class plane



    //------------------------------------
    // Default constructor for class plane
    //------------------------------------
    template <class T>
    plane<T>::plane() :
        shape<T>(),
        defining_point(0, 0, 0),
        normal(0, 0, 0),
        u_vector(0, 0, 0),
        v_vector(0, 0, 0),
        non_zero_u_index(0),
        non_zero_v_index(0)
    {

    } // plane()

    //---------------------------------------------------------
    // Secondary (pos/orientation) constructor for class plane.
    //---------------------------------------------------------
    template <class T>
    plane<T>::plane(const point3<T>& p, const vector3<T>& n) :
        shape<T>(),
        defining_point(p),
        normal(unit(n))
    {
        calculate_perpendicular_vectors(normal, u_vector, v_vector);

        setup_non_zero_indices();
    } // plane(point3,vector3)

    //---------------------------------------------------------
    // Tertiary (pos/orientation/rotation) constructor for class plane.
    //---------------------------------------------------------
    template <class T>
    plane<T>::plane(const point3<T>& p, const vector3<T>& n, const vector3<T>& vec_u) :
        shape<T>(),
        defining_point(p),
        normal(unit(n)),
        v_vector(crossprod(normal, vec_u))
    {
        u_vector = crossprod(v_vector, normal);

        setup_non_zero_indices();

    } // plane(point3,vector3,vector3)

    //---------------------------------------------------------
    // Quaternary (pos1, pos2, pos3) constructor for class plane.
    //---------------------------------------------------------
    template <class T>
    plane<T>::plane(const point3<T>& p, const point3<T>& plus_u, const point3<T>& plus_v) :
        shape<T>(),
        defining_point(p)
    {
        u_vector = plus_u - p;
        v_vector = plus_v - p;
        normal = unit(crossprod(u_vector, v_vector));

        setup_non_zero_indices();

    } // plane(point3,point3,point3)

    //---------------------------------------------------------
    // Quinary (pos, vec, vec, vec) constructor for class plane.
    //---------------------------------------------------------
    template <class T>
    plane<T>::plane(const point3<T>& p, const vector3<T>& normal,
                    const vector3<T>& vec_u, const vector3<T>& vec_v) :
        shape<T>(),
        defining_point(p),
        normal(unit(crossprod(vec_u, vec_v))),
        u_vector(vec_u),
        v_vector(vec_v)
    {

        setup_non_zero_indices();

    } // plane(point3,vector3,vector3,vector3)

    //---------------------------
    // Destructor for class plane
    //---------------------------
    template <class T>
    plane<T>::~plane()
    {

    } // ~plane()

    //---------------------------------
    // Copy constructor for class plane
    //---------------------------------
    template <class T>
    plane<T>::plane(const plane<T>& old) :
        shape<T>(old),
        defining_point(old.defining_point),
        normal(old.normal),
        u_vector(old.u_vector),
        v_vector(old.v_vector),
        non_zero_u_index(old.non_zero_u_index),
        non_zero_v_index(old.non_zero_v_index)
    {

    } // plane(plane)

    //------------------------------------
    // Assignment operator for class plane
    //------------------------------------
    template <class T>
    plane<T>& plane<T>::operator= (const plane<T>& old)
    {
        // Generic check for self-assignment
        if (&old != this)
        {
            defining_point = old.defining_point;
            normal = old.normal;
            u_vector = old.u_vector;
            v_vector = old.v_vector;

            non_zero_u_index = old.non_zero_u_index;
            non_zero_v_index = old.non_zero_v_index;
            shape<T>::operator=(old);
        }
        return *this;
    } // plane::operator=(plane)

    // Returns if the given point is inside the shape.
    template <class T>
    inline bool plane<T>::inside(const point3<T>& p) const
    {
        T dist = dotprod(p - defining_point, normal);
        return (dist < AMETHYST_EPSILON) && (dist > -AMETHYST_EPSILON);
    }

    // Returns if the given sphere intersects the shape.
    template <class T>
    inline bool plane<T>::intersects(const sphere<T>& s) const
    {
        T dist = dotprod(s.get_center() - defining_point, normal);
        T max_dist = s.get_radius() + AMETHYST_EPSILON;
        return (dist < max_dist) && (dist > -max_dist);
    }

    // Returns if the given plane intersects the shape.
    template <class T>
    inline bool plane<T>::intersects(const plane<T>& p) const
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
        else if (squared_length(defining_point - p.defining_point) <
                 AMETHYST_EPSILON)
        {
            return true;
        }
        // Anything else
        else
        {
            return false;
        }
    }


    // Returns if the given line intersects the plane.
    template <class T>
    inline bool plane<T>::intersects_line(const unit_line3<T>& line,
                                          intersection_info<T>& intersection,
                                          const intersection_requirements& requirements) const
    {
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
            t = (dotprod(defining_point - line.origin(), normal) /
                 ctheta);
        }
        else
        {
            t = (dotprod(defining_point - line.origin(), -normal) /
                 -ctheta);
        }

        if (line.inside(t))
        {
            intersection.set_shape(this);
            point3<T> p = line.point_at( t );
            intersection.set_ray( line );
            intersection.set_first_distance( t );
            intersection.set_first_point( p );

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

        return false;
    }


    // Returns if the given line intersects the plane.
    template <class T>
    inline bool plane<T>::quick_intersection(const unit_line3<T>& line,
                                             T time, T& distance) const
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
            t = (dotprod(defining_point - line.origin(), normal) /
                 ctheta);
        }
        else
        {
            t = (dotprod(defining_point - line.origin(), -normal) /
                 -ctheta);
        }

        if (line.inside(t))
        {
            distance = t;
            return true;
        }

        return false;
    }

    // Returns if the given line intersects the plane.
    template <class T>
    inline bool plane<T>::intersects_line(const line3<T>& line,
                                          intersection_info<T>& intersection,
                                          const intersection_requirements& requirements) const
    {
        T ctheta = (dotprod(line.direction(), normal) /
                    length(line.direction()));
        T t;

        // Check the unit line version of this function for proper comments on why
        // this works!
        if (ctheta > 0)
        {
            t = (dotprod(defining_point - line.origin(), normal) /
                 ctheta);
        }
        else
        {
            t = (dotprod(defining_point - line.origin(), -normal) /
                 -ctheta);
        }

        if (line.inside(t))
        {
            intersection.set_shape(this);
            unit_line3<T> unit_ray(line);
            T scaled_distance = t / unit_ray.normal_length();
            point3<T> p = unit_ray.point_at( scaled_distance );
            intersection.set_ray( unit_ray );
            intersection.set_first_distance( scaled_distance );
            intersection.set_first_point( p );

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

        return false;
    }

    template <class T>
    std::string plane<T>::internal_members(const std::string& indentation, bool prefix_with_classname) const
    {
        std::string retval;
        std::string internal_tagging = indentation;

        if (prefix_with_classname)
        {
            internal_tagging += plane<T>::name() + "::";
        }

        retval += indentation + string_format("intersection_capabilities=%1\n", get_intersection_capabilities().to_string());
        retval += indentation + string_format("object_capabilities=%1\n", get_object_capabilities().to_string());
        retval += internal_tagging + string_format("point=%1\n", defining_point);
        retval += internal_tagging + string_format("normal=%1\n", normal);
        retval += internal_tagging + string_format("u=%1\n", u_vector);
        retval += internal_tagging + string_format("v=%1\n", v_vector);
        retval += internal_tagging + string_format("nz_indices=(%1,%2)\n",
                                                   non_zero_u_index,
                                                   non_zero_v_index);
        return retval;
    }

    template <class T>
    void plane<T>::setup_non_zero_indices()
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

    template <class T>
    inline bool plane<T>::extract_uv_for_point(const point3<T>& point,
                                               coord2<T>& uv) const
    {
        if (plane<T>::inside(point))
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


    template <class T>
    inline void plane<T>::extract_uv_for_point_nonchecked(const point3<T>& point,
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


    template <class T>
    bool intersects(const sphere<T>& s, const plane<T>& p)
    {
        return p.intersects(s);
    }


    template <class T>
    intersection_capabilities plane<T>::get_intersection_capabilities() const
    {
        intersection_capabilities caps = shape<T>::get_intersection_capabilities();

        caps |= intersection_capabilities::HIT_FIRST;
        caps |= intersection_capabilities::NORMAL_CALCULATION;
        caps |= intersection_capabilities::UV_CALCULATION;
        caps |= intersection_capabilities::LOCAL_SYSTEM_CALCULATION;
        return caps;
    }

    template <class T>
    object_capabilities plane<T>::get_object_capabilities() const
    {
        object_capabilities caps = shape<T>::get_object_capabilities();

        caps |= object_capabilities::INFINITE;
        caps |= object_capabilities::SIMPLE;

        return caps;
    }

} // namespace amethyst


#endif /* !defined(AMETHYST__PLANE_HPP) */
