#pragma once
/*
   coord3.hpp

   A 3d coordinate class that has some basic functionality.  Also, some
   functions to act on that class (at the bottom).
 */

/*
   Revision history:
   17Jan1999 Wrote this file, did some basic testing added the dot product,
   cross product, length, and unit functions.
   20jan1999 Added a default constructor for the coord3 class and the
   coord3_union union.
   25jan1999 Added a unary - operator, and an invert function.
   05Mar1999 Added the * operator for two coord3s. It will take the
   componentwise product.
   02Apr1999 Added the constructor to take a single double, it is intended to
   init the coord to a given value.
   22Apr1999 Added the average function.
   Added the *= operator that takes two coord3s.
   18May2001 Started changing this for use in a newer tracer, as per "Realistic
   Raytracing" by Peter Shirley, "Templatized" the class...
   17Sep2003 Reordered the class, made it a part of namespace amethyst, changed
   extension to .hpp, added a GPL heading.
 */


#include <ostream>
#include <cmath>

namespace amethyst
{
    template <typename T>
    class coord3
    {
    public:
        enum COORD_VALUES
        {
            X = 0, Y = 1, Z = 2
        };
        typedef T base;

        coord3() = default;
        explicit coord3(T u) : coord3(u, u, u) { }
        constexpr coord3(T x, T y, T z)
            : m_x(x), m_y(y), m_z(z) { }
        coord3(const coord3& old_coord) = default;
        ~coord3() = default;

        void set(T x, T y, T z)
        {
            m_x = x;
            m_y = y;
            m_z = z;
        }

        /* Accessors */
        T& operator[](unsigned i) { return (i == X) ? m_x : ((i == Y) ? m_y : m_z); }
        constexpr T operator[](unsigned i) const { return (i == X) ? m_x : ((i == Y) ? m_y : m_z); }

        T& x() { return m_x; }
        constexpr T x() const { return m_x; }
        T& y() { return m_y; }
        constexpr T y() const { return m_y; }
        T& z() { return m_z; }
        constexpr T z() const { return m_z; }

        coord3& operator= (const coord3& old_coord) = default;
        coord3& operator+=(const coord3& p2);
        coord3& operator-=(const coord3& p2);
        coord3& operator*=(const coord3& p2); // Piecewise multiplication.
        coord3& operator*=(T factor);
        coord3& operator/=(T factor);
        template <class U>
        coord3& operator*=(U factor);
        template <class U>
        coord3& operator/=(U factor);

        inline T length() const
        {
            return T(std::sqrt(double((x() * x()) + (y() * y()) + (z() * z()))));
        }

    private:
        T m_x;
        T m_y;
        T m_z;
    };

    template <typename T>
    inline coord3<T>& coord3<T>::operator+=(const coord3<T>& p2)
    {
        m_x += p2.m_x;
        m_y += p2.m_y;
        m_z += p2.m_z;
        return *this;
    }

    template <typename T>
    inline coord3<T>& coord3<T>::operator-=(const coord3<T>& p2)
    {
        m_x -= p2.m_x;
        m_y -= p2.m_y;
        m_z -= p2.m_z;
        return *this;
    }

    // Piecewise multiplication...
    template <typename T>
    inline coord3<T>& coord3<T>::operator*=(const coord3<T>& p2)
    {
        m_x *= p2.m_x;
        m_y *= p2.m_y;
        m_z *= p2.m_z;
        return *this;
    }

    template <typename T>
    inline coord3<T>& coord3<T>::operator*=(T factor)
    {
        m_x *= factor;
        m_y *= factor;
        m_z *= factor;
        return *this;
    }

    template <typename T>
    inline coord3<T>& coord3<T>::operator/=(T factor)
    {
        m_x /= factor;
        m_y /= factor;
        m_z /= factor;
        return *this;
    }

    template <typename T>
    template <class U>
    inline coord3<T>& coord3<T>::operator*=(U factor)
    {
        m_x = T(m_x * factor);
        m_y = T(m_y * factor);
        m_z = T(m_z * factor);
        return *this;
    }

    template <typename T>
    template <class U>
    inline coord3<T>& coord3<T>::operator/=(U factor)
    {
        m_x = T(m_x / factor);
        m_y = T(m_y / factor);
        m_z = T(m_z / factor);
        return *this;
    }

    /* non-member math functions (also inlined for an attempt at some speed) */
    template <typename T>
    coord3<T> operator+(const coord3<T>& p1, const coord3<T>& p2)
    {
        return { p1.x() + p2.x(), p1.y() + p2.y(), p1.z() + p2.z() };
    }

    template <typename T>
    inline constexpr coord3<T> operator-(const coord3<T>& p1, const coord3<T>& p2)
    {
        return { p1.x() - p2.x(), p1.y() - p2.y(), p1.z() - p2.z() };
    }

    template <typename T, class U>
    inline constexpr coord3<T> operator*(const coord3<T>& p1, U factor)
    {
        return { p1.x() * factor, p1.y() * factor, p1.z() * factor };
    }

    template <typename T, class U>
    inline constexpr coord3<T> operator*(U factor, const coord3<T>& p1)
    {
        return { factor * p1.x(), factor * p1.y(), factor * p1.z() };
    }

    template <typename T>
    inline constexpr coord3<T> operator*(const coord3<T>& p1, const coord3<T>& p2)
    {
        return coord3<T>(p1.x() * p2.x(),
                         p1.y() * p2.y(),
                         p1.z() * p2.z());
    }

    template <typename T, class U>
    inline constexpr coord3<T> operator/(const coord3<T>& p1, U factor)
    {
        return { p1.x() / factor, p1.y() / factor, p1.z() / factor };
    }

    // Unary minus
    template <typename T>
    inline constexpr coord3<T> operator-(const coord3<T>& p1)
    {
        return { -p1.x(), -p1.y(), -p1.z() };
    }

    template <typename T>
    inline constexpr T dotprod(const coord3<T>& c1, const coord3<T>& c2)
    {
        return (c1.x() * c2.x()) + (c1.y() * c2.y()) + (c1.z() * c2.z());
    }

    template <typename T>
    inline constexpr coord3<T> crossprod(const coord3<T>& l, const coord3<T>& r)
    {
        return {
            (l.y() * r.z()) - (r.y() * l.z()),
            (r.x() * l.z()) - (l.x() * r.z()),
            (l.x() * r.y()) - (r.x() * l.y())
        };
    }

    template <typename T>
    inline constexpr T length(const coord3<T>& c)
    {
        return T(sqrt(dotprod(c, c)));
    }

    template <typename T>
    inline constexpr T squared_length(const coord3<T>& c)
    {
        return dotprod(c, c);
    }

    template <typename T>
    inline constexpr coord3<T> unit(const coord3<T>& c)
    {
        return c / length(c);
    }

    template <typename T>
    std::ostream& operator<<(std::ostream& o, const coord3<T>& c)
    {
        o << "{" << c.x() << "," << c.y() << "," << c.z() << "}";
        return o;
    }

    template <typename T>
    std::string inspect(const coord3<T>& c)
    {
        return "{" + inspect(c.x()) + "," + inspect(c.y()) + "," + inspect(c.z()) + "}";
    }
}
