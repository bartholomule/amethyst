/*
 * $Id: shape.hpp,v 1.12 2008/06/21 22:25:10 kpharris Exp $
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

#if       !defined(AMETHYST__SHAPE_HPP)
#define            AMETHYST__SHAPE_HPP

#include "amethyst/math/point3.hpp"
#include "amethyst/math/line3.hpp"
#include "amethyst/math/unit_line3.hpp"
#include "amethyst/graphics/intersection_info.hpp"
#include "amethyst/graphics/capabilities.hpp"
#include "amethyst/graphics/requirements.hpp"
#include "amethyst/graphics/ray_parameters.hpp"
#include "amethyst/general/string_dumpable.hpp"

namespace amethyst
{
    template <class T> class sphere;
    template <class T> class plane;
    template <class T> bool intersects(const sphere<T>& s, const plane<T>& p);

    /**
     *
     * The base class for a shape.
     *
     * @author Kevin Harris <kpharris@users.sourceforge.net>
     * @version $Revision: 1.12 $
     *
     */
    template <class T>
    class shape : public string_dumpable
    {

    private:

    protected:

    public:
        /** Default constructor */
        shape();

        /** Destructor */
        virtual ~shape();

        /** Copy constructor */
        shape(const shape& old);

        /** Assignment operator */
        shape& operator= (const shape& old);

        /** Returns if the given point is inside the shape. */
        virtual bool inside(const point3<T>& p) const = 0;

        /** Returns if the given sphere intersects the shape. */
        virtual bool intersects(const sphere<T>& s) const = 0;

        /** Returns if the given plane intersects the shape. */
        virtual bool intersects(const plane<T>& p) const = 0;

        /**
         * Returns if the given line intersects the shape.  For performance
         * reasons, this may be overridden, as the default just converts the line
         * to a unit line, calls the unit version, and adjusts the distance for the
         * scaling.
         */
        virtual bool intersects_line(const line3<T>& line,
                                     intersection_info<T>& intersection,
                                     const intersection_requirements& requirements = intersection_requirements()) const;

        /** Returns if the given line intersects the shape. */
        virtual bool intersects_line(const unit_line3<T>& line,
                                     intersection_info<T>& intersection,
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
        virtual bool intersects_ray(const ray_parameters<T>& ray,
                                    intersection_info<T>& intersection,
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

    }; // class shape



    //------------------------------------
    // Default constructor for class shape
    //------------------------------------
    template <class T>
    shape<T>::shape()
    {

    } // shape()

    //---------------------------
    // Destructor for class shape
    //---------------------------
    template <class T>
    shape<T>::~shape()
    {

    } // ~shape()

    //---------------------------------
    // Copy constructor for class shape
    //---------------------------------
    template <class T>
    shape<T>::shape(const shape<T>& old)
    {

    } // shape(shape)

    //------------------------------------
    // Assignment operator for class shape
    //------------------------------------
    template <class T>
    shape<T>& shape<T>::operator= (const shape<T>& old)
    {
        // Generic check for self-assignment
        if (&old != this)
        {
            // Insert assignment here. DELETEME

        }
        return *this;
    } // shape::operator=(shape)

    template <class T>
    bool shape<T>::intersects_line(const line3<T>& line,
                                   intersection_info<T>& intersection,
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
    } // shape::intersects_line(line3<T>,T)


    template <class T>
    bool shape<T>::intersects_ray(const ray_parameters<T>& ray,
                                  intersection_info<T>& intersection,
                                  const intersection_requirements& requirements) const
    {
        if (intersects_line(ray.get_line(), intersection, requirements))
        {
            return true;
        }
        return false;
    } // shape::intersects_ray(line3<T>,T)

} // namespace amethyst


#endif /* !defined(AMETHYST__SHAPE_HPP) */
