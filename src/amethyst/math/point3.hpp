#pragma once
/*
   point3.hpp

   An expansion of the coord3 class to allow the creation of distinct types
   which will somewhat prevent a user from doing anything stupid.

   It allows for creation of a vector by subtracting two points, and creation of
   another point by addition or subtraction of a vector to a point.
 */

/*
   Revision history:
   17jan1999 Wrote this file, did some basic testing.  Added the extraction
   operator to this, and the other '3' classes (vector3, coord3).
   This will make different output when dumped on a stream.  It could
   come in handy when debugging something.
   20jan1999 Added a default constructor.
   18May2001 "Templatized" the class...
   17Sep2003 Changed the inheritence from public to private.  Added accessors,
   fixed implementation to use those accessors.
 */

#include "coord3.hpp"
#include "vector3.hpp"

namespace amethyst
{

    template <typename T>
    class point3 : private coord3<T>
    {
    public:
        using parent = coord3<T>;

        point3() = default;
        constexpr explicit point3(const coord3<T>& c) : parent(c) { }
        constexpr point3(T a, T b, T c) : parent(a, b, c) { }
        operator coord3<T>() const { return *this; }
        point3(const point3&) = default;

        coord3<T> getcoord() const
        {
            return coord3<T>(x(), y(), z());
        }

        using parent::set;
        using parent::operator[];
        using parent::x;
        using parent::y;
        using parent::z;

        point3<T>& operator=(const point3<T>& p) = default;
        point3<T>& operator+=(const vector3<T>& v1)
        {
            set(x() + v1.x(), y() + v1.y(), z() + v1.z());
            return *this;
        }
        point3<T>& operator-=(const vector3<T>& v1)
        {
            set(x() - v1.x(), y() - v1.y(), z() - v1.z());
            return *this;
        }
    };

    template <typename T>
    inline constexpr vector3<T> operator-(const point3<T>& p1, const point3<T>& p2)
    {
        return { p1.x() - p2.x(), p1.y() - p2.y(), p1.z() - p2.z() };
    }
    template <typename T>
    inline constexpr point3<T> operator+(const point3<T>& p1, const vector3<T>& v1)
    {
        return { p1.x() + v1.x(), p1.y() + v1.y(), p1.z() + v1.z() };
    }
    template <typename T>
    inline constexpr point3<T> operator-(const point3<T>& p1, const vector3<T>& v1)
    {
        return { p1.x() - v1.x(), p1.y() - v1.y(), p1.z() - v1.z() };
    }
    template <typename T>
    inline constexpr point3<T> operator+(const vector3<T>& v1, const point3<T>& p1)
    {
        return { p1.x() + v1.x(), p1.y() + v1.y(), p1.z() + v1.z() };
    }
    template <typename T>
    inline constexpr point3<T> operator-(const vector3<T>& v1, const point3<T>& p1)
    {
        return { v1.x() - p1.x(), v1.y() - p1.y(), v1.z() - p1.z() };
    }

    template <typename T>
    std::ostream& operator<<(std::ostream& o, const point3<T>& p)
    {
        o << "(" << p.x() << "," << p.y() << "," << p.z() << ")";
        return o;
    }

    template <typename T>
    std::string inspect(const point3<T>& p)
    {
        return "(" + inspect(p.x()) + "," + inspect(p.y()) + "," + inspect(p.z()) + ")";
    }
}