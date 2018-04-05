/*
 * $Id: triangle.hpp,v 1.10 2008/06/21 22:25:10 kpharris Exp $
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

#if       !defined(AMETHYST__TRIANGLE_HPP)
#define            AMETHYST__TRIANGLE_HPP

// --------------------------------------
// Include of parent class header
// --------------------------------------
#include "amethyst/graphics/plane.hpp"

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
    template <class T>
    class triangle : public plane<T>
    {

    private:

    protected:

    public:
        /** Default constructor */
        triangle();

        triangle(const point3<T>& corner1 = point3<T>(0, 0, 0),
                 const point3<T>& corner2 = point3<T>(1, 0, 0),
                 const point3<T>& corner3 = point3<T>(0, 1, 0));

        /** Destructor */
        virtual ~triangle();

        /** Copy constructor */
        triangle(const triangle& old);

        /** Assignment operator */
        triangle& operator= (const triangle& old);

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
            return "triangle";
        }

        virtual intersection_capabilities get_intersection_capabilities() const;
        virtual object_capabilities get_object_capabilities() const;

    }; // class triangle



    //---------------------------------------
    // Default constructor for class triangle
    //---------------------------------------
    template <class T>
    triangle<T>::triangle() :
        plane<T>()
    {

    } // triangle()

    template <class T>
    triangle<T>::triangle(const point3<T>& corner1,
                          const point3<T>& corner2,
                          const point3<T>& corner3) :
        plane<T>(corner1, corner2, corner3)
    {
    } // triangle(point3,point3,point3)


    //------------------------------
    // Destructor for class triangle
    //------------------------------
    template <class T>
    triangle<T>::~triangle()
    {

    } // ~triangle()

    //------------------------------------
    // Copy constructor for class triangle
    //------------------------------------
    template <class T>
    triangle<T>::triangle(const triangle<T>& old) :
        plane<T>(old)
    {

    } // triangle(triangle)

    //---------------------------------------
    // Assignment operator for class triangle
    //---------------------------------------
    template <class T>
    triangle<T>& triangle<T>::operator= (const triangle<T>& old)
    {
        // Generic check for self-assignment
        if (&old != this)
        {
            plane<T>::operator=(old);
        }
        return *this;
    } // triangle::operator=(triangle)

    template <class T>
    bool triangle<T>::inside(const point3<T>& point) const
    {
        coord2<T> uv;
        if (plane<T>::extract_uv_for_point(point, uv))
        {
            if ((uv.x() > 0 && uv.y() > 0) &&
                (uv.x() + uv.y() < 1))
            {
                return true;
            }
        }
        return false;
    }

    template <class T>
    bool triangle<T>::intersects(const sphere<T>& s) const
    {
        // If any of the 3 corners is inside the sphere, the triangle intersects.
        if (s.inside( plane<T>::get_origin()) ||
            s.inside( plane<T>::get_origin() + plane<T>::get_u_vector()) ||
            s.inside( plane<T>::get_origin() + plane<T>::get_v_vector()))
        {
            return true;
        }
        return false;
    }

    template <class T>
    bool triangle<T>::intersects(const plane<T>& p) const
    {
        T unused;
        const point3<T>& pp = plane<T>::get_origin();
        point3<T> pu = plane<T>::get_origin() + plane<T>::get_u_vector();
        point3<T> pv = plane<T>::get_origin() + plane<T>::get_v_vector();

        if (p.quick_intersection( unit_line3<T>(pp, pu), 0, unused ) ||
            p.quick_intersection( unit_line3<T>(pp, pv), 0, unused ) ||
            p.quick_intersection( unit_line3<T>(pu, pv), 0, unused ))
        {
            return true;
        }

        return false;
    }

    template <class T>
    bool triangle<T>::intersects_line(const unit_line3<T>& line,
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

    template <class T>
    std::string triangle<T>::internal_members(const std::string& indentation, bool prefix_with_classname) const
    {
        std::string retval = plane<T>::internal_members(indentation, prefix_with_classname);
        std::string internal_tagging = indentation;

        if (prefix_with_classname)
        {
            internal_tagging += triangle<T>::name() + "::";
        }

        return retval;
    }

    template <class T>
    intersection_capabilities triangle<T>::get_intersection_capabilities() const
    {
        intersection_capabilities caps = plane<T>::get_intersection_capabilities();

        return caps;
    }

    template <class T>
    object_capabilities triangle<T>::get_object_capabilities() const
    {
        object_capabilities caps = plane<T>::get_object_capabilities();

        caps &= ~object_capabilities::NOT_FINITE;
        caps |= object_capabilities::BOUNDABLE;
        caps |= object_capabilities::POLYGONIZATION;

        return caps;
    }

    template <class T>
    bool triangle<T>::quick_intersection(const unit_line3<T>& line, T time, T& distance) const
    {
        T temp_distance;

        if (plane<T>::quick_intersection(line, time, temp_distance))
        {
            point3<T> p = (temp_distance * line.direction() + line.origin());
            coord2<T> uv;

            // Note that a non-checked version is being called, because the point
            // *SHOULD* be on the plane (it hit it, why check it?).  This improves
            // speed, and reduces artifacts with expanded error (a point that is on
            // the plane would sometimes not show as being on the plane -- this is
            // the case when a small epsilon and single-precision floats are used.
            plane<T>::extract_uv_for_point_nonchecked(p, uv);

            if ((uv.x() > 0 && uv.y() > 0) &&
                (uv.x() + uv.y() < 1))
            {
                distance = temp_distance;
                return true;
            }
        }
        return false;
    }

} // namespace amethyst


#endif /* !defined(AMETHYST__TRIANGLE_HPP) */
