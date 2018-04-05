#pragma once
/*
   vector3.hpp

   This goes along with the point3 class, and has very few internal functions.
   It is intended for use as you would any vector.

 */


/*
   Revision history:
   17jan1999 Wrote this file, did some basic testing.
   20jan1999 Added a default constructor.
   09Mar1999 Added the operator* to make the sgi compiler happy.  It was
   complaining that there were multiple ways to do the multiplication
   operation by converting the vector to other types.
   Added the - operator for the same reason.
   18May2001 "Templatized" the class...
   17Sep2003 Changed the inheritence from public to private.  Added accessors,
   fixed implementation to use those accessors.
 */

#include "coord3.hpp"
#include "vector_utils.hpp"

namespace amethyst
{
    template <class T>
    class vector3 : private coord3<T>
    {
    private:
        using parent = coord3<T>;
    public:
        typedef T base;

        vector3() = default;
        vector3(const vector3<T>&) = default;
        inline constexpr vector3(T x, T y, T z) : parent(x, y, z) { }
        inline explicit vector3(const parent& c) : parent(c) { }
        vector3& operator=(const vector3&) = default;

        using parent::set;
        using parent::operator[];
        using parent::x;
        using parent::y;
        using parent::z;


        inline vector3& operator+=(const vector3& p2) { parent::operator+=(p2); return *this; }
        inline vector3& operator-=(const vector3& p2) { parent::operator-=(p2); return *this; }
        template <typename U>
        inline vector3& operator*=(U factor) { parent::operator*=(factor); return *this; }
        template <typename U>
        inline vector3& operator/=(U factor) { parent::operator*=(factor); return *this; }

    };

    template <class T>
    inline constexpr vector3<T> operator-(const vector3<T>& v1)
    {
        return { -v1.x(), -v1.y(), -v1.z() };
    }

    template <class T, typename U>
    inline constexpr vector3<T> operator*(const vector3<T>& v, U factor)
    {
        return { v.x() * factor, v.y() * factor, v.z() * factor };
    }

    template <class T, typename U>
    inline constexpr vector3<T> operator*(U factor, const vector3<T>& v)
    {
        return { v.x() * factor, v.y() * factor, v.z() * factor };
    }

    template <class T, typename U>
    inline constexpr vector3<T> operator/(const vector3<T>& v, U factor)
    {
        return { v.x() / factor, v.y() / factor, v.z() / factor };
    }

    template <class T>
    inline constexpr vector3<T> operator-(const vector3<T>& v1, const vector3<T>& v2)
    {
        return { v1.x() - v2.x(), v1.y() - v2.y(), v1.z() - v2.z() };
    }

    template <class T>
    inline constexpr vector3<T> operator+(vector3<T> v1, const vector3<T>& v2)
    {
        return { v1.x() + v2.x(), v1.y() + v2.y(), v1.z() + v2.z() };
    }

    template <class T>
    inline constexpr vector3<T> crossprod(const vector3<T>& l, const vector3<T>& r)
    {
        return {
            (l.y() * r.z()) - (r.y() * l.z()),
            (r.x() * l.z()) - (l.x() * r.z()),
            (l.x() * r.y()) - (r.x() * l.y())
        };
    }

    template <typename T>
    inline constexpr T dotprod(const vector3<T>& v1, const vector3<T>& v2)
    {
        return (v1.x() * v2.x()) + (v1.y() * v2.y()) + (v1.z() * v2.z());
    }

    // Can't be constexpr because std::sqrt isn't constexpr, and I can't select my version at compile time.
    template <class T>
    inline T length(const vector3<T>& v)
    {
        return sqrt(dotprod(v, v));
    }

    template <class T>
    inline constexpr T squared_length(const vector3<T>& v)
    {
        return dotprod(v, v);
    }

    // Also can't be constexpr.
    template <class T>
    inline vector3<T> unit(const vector3<T>& v)
    {
        return v / length(v);
    }

    template <class T>
    std::ostream& operator<< (std::ostream& o, const vector3<T>& v)
    {
        o << "<" << v.x() << "," << v.y() << "," << v.z() << ">";
        return o;
    }

    template <typename T>
    std::string inspect(const vector3<T>& v)
    {
        return "<" + inspect(v.x()) + "," + inspect(v.y()) + "," + inspect(v.z()) + ">";
    }
}
