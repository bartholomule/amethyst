/*
 * $Id: base_camera.hpp,v 1.4 2008/06/21 22:25:10 kpharris Exp $
 *
 * Part of "Amethyst" -- A playground for graphics development.
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

#if       !defined(AMETHYST__BASE_CAMERA_HPP)
#define            AMETHYST__BASE_CAMERA_HPP

#include "amethyst/math/unit_line3.hpp"
#include "amethyst/general/string_format.hpp"
#include "amethyst/general/string_dumpable.hpp"

namespace amethyst
{
    /**
     *
     * A base class for a camera.
     *
     * @author Kevin Harris <kpharris@users.sourceforge.net>
     * @version $Revision: 1.4 $
     *
     */
    template <class T>
    class base_camera : public string_dumpable
    {

    private:
        size_t nx;
        size_t ny;

    protected:

    public:
        /** Default constructor */
        base_camera();

        /** Secondary constructor */
        base_camera(size_t w, size_t h);

        /** Destructor */
        virtual ~base_camera();

        /** Copy constructor */
        base_camera(const base_camera& old);

        /** Assignment operator */
        base_camera& operator= (const base_camera& old);

        /* Width and height of the image in pixels. */
        size_t width() const {
            return nx;
        }
        size_t height() const {
            return ny;
        }

        void set_width(size_t w) {
            nx = w;
        }
        void set_height(size_t h) {
            ny = h;
        }

        /* FIXME! At some point in the future, these will need to have a more
           complete ray, which includes time, etc. */
        /* This sample point is in [0,1]^2 */
        virtual ray_parameters<T> get_ray(const coord2<T>& sample_point, T time = 0) const = 0;
        /* px, py are the pixel positions. */
        virtual ray_parameters<T> get_ray(const T& px, const T& py, T time = 0) const = 0;

        virtual std::string internal_members(const std::string& indentation, bool prefix_with_classname = false) const;

        virtual std::string name() const {
            return "base_camera";
        }
    }; // class base_camera



    //------------------------------------------
    // Default constructor for class base_camera
    //------------------------------------------
    template <class T>
    base_camera<T>::base_camera() :
        nx(0), ny(0)
    {

    } // base_camera()

    //------------------------------------------
    // Secondary constructor for class base_camera
    //------------------------------------------
    template <class T>
    base_camera<T>::base_camera(size_t w, size_t h) :
        nx(w), ny(h)
    {

    } // base_camera(w,h)

    //---------------------------------
    // Destructor for class base_camera
    //---------------------------------
    template <class T>
    base_camera<T>::~base_camera()
    {

    } // ~base_camera()

    //---------------------------------------
    // Copy constructor for class base_camera
    //---------------------------------------
    template <class T>
    base_camera<T>::base_camera(const base_camera<T>& old) :
        nx(old.nx),
        ny(old.ny)
    {

    } // base_camera(base_camera)

    //------------------------------------------
    // Assignment operator for class base_camera
    //------------------------------------------
    template <class T>
    base_camera<T>& base_camera<T>::operator= (const base_camera<T>& old)
    {
        // Generic check for self-assignment
        if (&old != this)
        {
            nx = old.nx;
            ny = old.ny;
        }
        return *this;
    } // base_camera::operator=(base_camera)

    template <class T>
    std::string base_camera<T>::internal_members(const std::string& indentation, bool prefix_with_classname) const
    {
        std::string retval;

        std::string internal_tagging = indentation;

        if (prefix_with_classname)
        {
            internal_tagging += base_camera<T>::name() + "::";
        }
        retval += internal_tagging + string_format("width=%1\n", nx);
        retval += internal_tagging + string_format("height=%1\n", ny);
        return retval;
    }

} // namespace amethyst


#endif /* !defined(AMETHYST__BASE_CAMERA_HPP) */

