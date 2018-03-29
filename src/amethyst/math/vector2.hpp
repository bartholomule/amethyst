#pragma once
/*
   vector2.hpp

   The 2d equiv of the stuff in vector3.hpp
 */

/*
   Revision history:
   17Jan1999 Wrote this file, did some basic testing.
   20jan1999 Added a default constructor.
   18May2001 "Templatized" the class...
 */

#include "coord2.hpp"
#include "vector_utils.hpp"

namespace amethyst
{
    template <class T>
    class vector2 : private coord2<T>
    {
    private:
        using parent = coord2<T>;
    public:
        typedef T base;

        vector2() = default;
        vector2(const vector2&) = default;
        vector2(T x, T y) : parent(x, y) { }
        explicit vector2(const coord2<T>& c) : coord2<T>(c) { }
        vector2& operator=(const vector2&) = default;

        using parent::set;
        using parent::operator[];
        using parent::x;
        using parent::y;

        vector2& operator+=(const vector2& v1) { parent::operator+=(v1); return *this; }
        vector2& operator-=(const vector2& v1) { parent::operator-=(v1); return *this; }
        template <class U>
        vector2& operator*=(U factor) { parent::operator*=(factor); return *this; }
        template <class U>
        vector2& operator/=(U factor) { parent::operator/=(factor); return *this; }
    };

    template <class T>
    inline vector2<T> operator-(const vector2<T>& v1)
    {
        return T(-1) * v1;
    }

    template <class T>
    inline vector2<T> operator-(vector2<T> v1, const vector2<T>& v2)
    {
        v1 -= v2;
        return v1;
    }

    template <class T>
    inline vector2<T> operator+(vector2<T> v1, const vector2<T>& v2)
    {
        v1 += v2;
        return v1;
    }

    template <class T, class U>
    inline vector2<T> operator*(vector2<T> v, U factor)
    {
        v *= factor;
        return v;
    }

    template <class T, class U>
    inline vector2<T> operator*(U factor, vector2<T> v)
    {
        v *= factor;
        return v;
    }

    template <class T, class U>
    inline vector2<T> operator/(vector2<T> v, U factor)
    {
        v /= factor;
        return v;
    }

    template <typename T>
    inline T dotprod(const vector2<T>& v1, const vector2<T>& v2)
    {
        return (v1.x() * v2.x()) + (v1.y() * v2.y());
    }

    template <typename T>
    inline T length(const vector2<T>& v)
    {
        return T(sqrt(dotprod(v, v)));
    }

    // FIXME! Find an exception to throw...
    template <class T>
    inline vector2<T> unit(const vector2<T>& v)
    {
        T len = length(v);
        // FIXME! Check the squared length >= 0
        return v / len;
    }

    template <class T>
    std::ostream& operator<<(std::ostream& o, const vector2<T>& v)
    {
        o << "<" << v.x() << "," << v.y() << ">";
        return o;
    }

    template <typename T>
    std::string inspect(const vector2<T>& v)
    {
        return "<" + inspect(v.x()) + "," + inspect(v.y()) + ">";
    }
}




