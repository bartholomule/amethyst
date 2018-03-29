/*
 * $Id: line2.hpp,v 1.5 2004/04/18 21:40:04 kpharris Exp $
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

#if       !defined(AMETHYST__LINE2_HPP)
#define            AMETHYST__LINE2_HPP

#include "point2.hpp"
#include "vector2.hpp"
#include "interval.hpp"

#include <limits>

namespace amethyst
{

    /**
     *
     * A simple 2d line.
     *
     * @author Kevin Harris <kpharris@users.sourceforge.net>
     * @version $Revision: 1.5 $
     *
     */
    template <class T>
    class line2
    {

    private:

    protected:
        point2<T> line_origin;
        vector2<T> line_direction;
        interval<T> line_limits;
        bool infinite_limits;

    public:
        /** Default constructor */
        line2();

        line2(const point2<T>& o, const vector2<T>& v,
              const interval<T>& limits = interval<T>(numeric_limits<T>::min(),
                                                      numeric_limits<T>::max()));
        line2(const point2<T>& p1, const point2<T>& p2);

        /** Destructor */
        virtual ~line2();

        /** Copy constructor */
        line2(const line2& old);

        /** Assignment operator */
        line2& operator= (const line2& old);

        point2<T> o() const
        {
            return line_origin;
        }
        point2<T> origin() const
        {
            return line_origin;
        }

        vector2<T> v() const
        {
            return line_direction;
        }
        vector2<T> direction() const
        {
            return line_direction;
        }

        point2<T> minimum() const
        {
            return line_origin +
                   line_limits.begin() * direction();
        }
        point2<T> maximum() const
        {
            return line_origin +
                   line_limits.end() * direction();
        }
        point2<T> point_at(const T& t) const
        {
            return line_origin + direction() * t;
        }

        bool infinite() const
        {
            return infinite_limits;
        }

        interval<T> limits() const
        {
            return line_limits;
        }

        bool inside(T t) const
        {
            return line_limits.inside(t);
        }

    }; // class line2



    //-----------------------------------
    // Default constructor for class line2
    //-----------------------------------
    template <class T>
    line2<T>::line2() :
        line_origin(),
        line_direction(),
        line_limits( -numeric_limits<T>::max(),
                     numeric_limits<T>::max ),
        infinite_limits(true)
    {

    } // line2()

    template <class T>
    line2<T>::line2(const point2<T>& o, const vector2<T>& v,
                    const interval<T>& limits) :
        line_origin(o),
        line_direction(v),
        line_limits(limits)
    {
        // Yes, I know that floating-point comparisons of this sort is usually bad,
        // but if they set it to something other than the default, this should
        // fail, so this wouldn't be infinite.
        infinite_limits = ((line_limits.begin() <= -numeric_limits<T>::max()) &&
                           (line_limits.end() >= numeric_limits<T>::max()));

    } // line2()

    template <class T>
    line2<T>::line2(const point2<T>& p1, const point2<T>& p2) :
        line_origin(p1),
        line_direction(p2 - p1),
        line_limits( numeric_limits<T>::min(),
                     T(1) - numeric_limits<T>::epsilon()),
        infinite_limits(false)
    {

    } // line2()


    //--------------------------
    // Destructor for class line2
    //--------------------------
    template <class T>
    line2<T>::~line2()
    {

    } // ~line2()

    //--------------------------------
    // Copy constructor for class line2
    //--------------------------------
    template <class T>
    line2<T>::line2(const line2<T>& old) :
        line_origin(old.line_origin),
        line_direction(old.line_direction),
        line_limits(old.line_limits),
        infinite_limits(old.infinite_limits)
    {

    } // line2(line2)

    //-----------------------------------
    // Assignment operator for class line2
    //-----------------------------------
    template <class T>
    line2<T>& line2<T>::operator= (const line2<T>& old)
    {
        // Generic check for self-assignment
        if (&old != this)
        {
            line_origin = old.line_origin;
            line_direction = old.line_direction;
            line_limits = old.line_limits;
            infinite_limits = old.infinite_limits;
        }
        return *this;
    } // line2::operator=(line2)

    template <class T>
    std::ostream& operator<<(std::ostream& o, const line2<T>& l)
    {
        o << "|" << l.o() << ", " << l.v() << "|";
        return o;
    }

    template <typename T>
    std::string inspect(const line2<T>& l)
    {
        return "|" + inspect(l.o()) + ", " + inspect(l.v()) + "|";
    }

} // namespace amethyst


#endif /* !defined(AMETHYST__LINE2_HPP) */

