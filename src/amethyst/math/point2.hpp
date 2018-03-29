#pragma once
/*
   point2.hpp

   A 2d equiv of the stuff in point3.hpp
 */

/*
   revision history:
   17Jan1999 Wrote this file, did some basic testing.
   20jan1999 Added a default constructor.
   02feb1999 Fixed the broken constructor.
   18May2001 "Templatized" the class...
   17Sep2003 Changed the inheritence from public to private.  Added accessors,
   fixed implementation to use those accessors.
 */

#include "coord2.hpp"
#include "vector2.hpp"

namespace amethyst
{

    template <typename T>
    class point2 : private coord2<T>
    {
    public:
        using parent = coord2<T>;

        point2() = default;
        point2(const point2& p) = default;
        inline point2(T x, T y) : parent(x, y) { }
        inline explicit point2(const coord2<T>& c) : parent(c) { }

        coord2<T> getcoord() const
        {
            return coord2<T>(x(), y());
        }

        using parent::set;
        using parent::operator[];
        using parent::x;
        using parent::y;

        point2<T>& operator=(const point2<T>& p) = default;
        point2<T>& operator+=(const vector2<T>& v1);
        point2<T>& operator-=(const vector2<T>& v1);
    };


    template <typename T>
    inline point2<T>& point2<T>::operator+=(const vector2<T>& v1)
    {
        set(x() + v1.x(), y() + v1.y());
        return *this;
    }

    template <typename T>
    inline point2<T>& point2<T>::operator-=(const vector2<T>& v1)
    {
        set(x() - v1.x(), y() - v1.y());
        return *this;
    }

    template <typename T>
    inline vector2<T> operator-(const point2<T>& p1, const point2<T>& p2)
    {
        return vector2<T>(p1.x() - p2.x(), p1.y() - p2.y());
    }

    template <typename T>
    inline point2<T> operator+(const point2<T>& p1, const vector2<T>& v1)
    {
        return point2<T>(p1.x() + v1.x(), p1.y() + v1.y());
    }

    template <typename T>
    inline point2<T> operator-(const point2<T>& p1, const vector2<T>& v1)
    {
        return point2<T>(p1.x() - v1.x(), p1.y() - v1.y());
    }

    template <typename T>
    std::ostream& operator<< (std::ostream& o, const point2<T>& p)
    {
        o << "(" << p.x() << "," << p.y() << ")";
        return o;
    }

    template <typename T>
    std::string inspect(const point2<T>& p)
    {
        return "(" + inspect(p.x()) + "," + inspect(p.y()) + ")";
    }
}