#pragma once
/*
   coord2.hpp

   The 2d equiv of everything that I placed initially in coord3.h, except for
   the cross product, as there is no 2d equiv.
 */

/*
   Revision history:
   17Jan1999 Wrote this file, did some basic testing.
   20jan1999 Added a default constructor for the coord2 class.
   Added a default constructor for the union in the coord2 class.
   25jan1999 Added a unary - operator, and an invert function.
   18May2001 "Templatized" the class...
   17Sep2003 Reordered the class, made it a part of namespace amethyst, changed
   extension to .hpp, added a GPL heading.
 */


#include <ostream>
#include <algorithm>
#include "../general/template_functions.hpp"

namespace amethyst
{

    template <class T>
    class coord2
    {
    private:
        /* class that has 2 Ts stored directly */
        struct coord2_direct
        {
            T x; T y;
        };
        /* class that has 2 Ts stored in an array */
        struct coord2_array
        {
            T coords[2];
        };
        /* union to allow accesses to both indirectly through an array, and directly
           through a name, without adding any extra processing time or space
           requirements */
        union coord2_union
        {
            coord2_union() = default;
            coord2_union(const coord2_union& old)
                : direct{ old.direct.x, old.direct.y }
            {
            }

            coord2_union(T x, T y) { direct.x = x; direct.y = y; }
            T& operator[](int index) { return array.coords[index]; }
            T operator[](int index) const { return array.coords[index]; }
            coord2_union& operator=(const coord2_union& old)
            {
                direct.x = old.direct.x;
                direct.y = old.direct.y;
                return *this;
            }

            coord2_direct direct;
            coord2_array array;
        };

        /* The internal coordinates of this class */
        coord2_union coords;

    public:
        enum COORD_VALUES
        {
            X = 0, Y = 1
        };
        typedef T base;

        coord2() = default;
        coord2(T x, T y) : coords(x, y) { }
        coord2(const coord2& old_coord) : coords(old_coord.coords) { }

        void set(T x, T y)
        {
            coords.direct.x = x;
            coords.direct.y = y;
        }

        /* Accessors */
        T& operator[](int coord_index) { return coords[coord_index]; }
        T operator[](int coord_index) const { return coords[coord_index]; }
        T& x() { return coords.direct.x; }
        T x() const { return coords.direct.x; }
        T& y() { return coords.direct.y; }
        T y() const { return coords.direct.y; }

        coord2& operator=(const coord2& old) { coords = old.coords; return *this; }
        coord2& operator+=(const coord2& p2)
        {
            coords.direct.x += p2.coords.direct.x;
            coords.direct.y += p2.coords.direct.y;
            return *this;
        }
        coord2& operator-=(const coord2& p2)
        {
            coords.direct.x -= p2.coords.direct.x;
            coords.direct.y -= p2.coords.direct.y;
            return *this;
        }
        coord2& operator*=(const coord2& p2) // Piecewise multiplication.
        {
            coords.direct.x *= p2.coords.direct.x;
            coords.direct.y *= p2.coords.direct.y;
            return *this;
        }
        template <class U>
        coord2& operator*=(U factor)
        {
            coords.direct.x = T(coords.direct.x * factor);
            coords.direct.y = T(coords.direct.y * factor);
            return *this;
        }
        template <class U>
        coord2& operator/=(U factor)
        {
            coords.direct.x = T(coords.direct.x / factor);
            coords.direct.y = T(coords.direct.y / factor);
            return *this;
        }
        /* Functions that more relate to vectors, but are needed */
        inline T length() const
        {
            return T(sqrt(double(x() * x() + y() * y())));
        }
    };

    /* non-member math functions (also inlined for an attempt at some speed) */
    template <class T>
    inline coord2<T> operator+(coord2<T> p1, const coord2<T>& p2)
    {
        p1 += p2;
        return p1;
    }
    template <class T>
    inline coord2<T> operator-(coord2<T> p1, const coord2<T>& p2)
    {
        p1 -= p2;
        return p1;
    }
    template <class T, typename U>
    inline coord2<T> operator*(coord2<T> p1, U d)
    {
        p1 *= d;
        return p1;
    }
    template <class T, typename U>
    inline coord2<T> operator*(U d, coord2<T> p1)
    {
        p1 *= d;
        return p1;
    }
    template <class T, typename U>
    inline coord2<T> operator/(coord2<T> p1, U d)
    {
        p1 /= d;
        return p1;
    }

    template <class T>
    inline coord2<T> operator*(coord2<T> p1, const coord2<T>& p2)
    {
        p1 *= p2;
        return p1;
    }

    /* Unary minus */
    template <class T>
    inline coord2<T> operator-(const coord2<T>& p1)
    {
        return T(-1) * p1;
    }

    template <class T>
    inline T dotprod(const coord2<T>& c1, const coord2<T>& c2)
    {
        return (c1.x() * c2.x()) + (c1.y() * c2.c.y());
    }

    template <class T>
    inline T length(const coord2<T>& c)
    {
        return c.length();
    }

    template <class T>
    inline coord2<T> unit(const coord2<T>& c)
    {
        return c / c.length();
    }

    template <class T>
    std::ostream& operator<< (std::ostream& o, const coord2<T>& c)
    {
        o << "{" << c.x() << "," << c.y() << "}";
        return o;
    }

    template <typename T>
    std::string inspect(const coord2<T>& c)
    {
        return "{" + inspect(c.x()) + "," + inspect(c.y()) + "}";
    }
}
