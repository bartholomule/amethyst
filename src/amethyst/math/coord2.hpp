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
#include <cmath>
#include "../general/template_functions.hpp"

namespace amethyst
{

    template <class T>
    class coord2
    {
    private:
        T m_x;
        T m_y;

    public:
        enum COORD_VALUES
        {
            X = 0, Y = 1
        };
        typedef T base;

        coord2() = default;
        coord2(T x, T y) : m_x(x), m_y(y) { }
        coord2(const coord2&) = default;

        void set(T x, T y)
        {
            m_x = x;
            m_y = y;
        }

        /* Accessors */
        T& operator[](unsigned i) { return (i == X) ? m_x : m_y; }
        constexpr T operator[](unsigned i) const { return (i == X) ? m_x : m_y; }
        constexpr T x() const { return m_x; }
        constexpr T y() const { return m_y; }
        inline void set_x(T x) { m_x = x; }
        inline void set_y(T y) { m_y = y; }

        void swap_x(coord2<T>& other) { std::swap(m_x, other.m_x); }
        void swap_y(coord2<T>& other) { std::swap(m_y, other.m_y); }

        coord2& operator=(const coord2& old) = default;
        coord2& operator+=(const coord2& p2)
        {
            m_x += p2.m_x;
            m_y += p2.m_y;
            return *this;
        }
        coord2& operator-=(const coord2& p2)
        {
            m_x -= p2.m_x;
            m_y -= p2.m_y;
            return *this;
        }
        coord2& operator*=(const coord2& p2) // Piecewise multiplication.
        {
            m_x *= p2.m_x;
            m_y *= p2.m_y;
            return *this;
        }
        template <class U>
        coord2& operator*=(U factor)
        {
            m_x = T(m_x * factor);
            m_y = T(m_y * factor);
            return *this;
        }
        template <class U>
        coord2& operator/=(U factor)
        {
            if constexpr (
                std::is_same<T, double>::value || std::is_same<T, float>::value
                || std::is_same<U, double>::value || std::is_same<U, float>::value)
            {
                auto v = T(1) / factor;
                m_x = T(m_x * v);
                m_y = T(m_y * v);
            }
            else
            {
                m_x = T(m_x / factor);
                m_y = T(m_y / factor);
            }
            return *this;
        }
        /* Functions that more relate to vectors, but are needed */
        inline T length() const
        {
            return T(sqrt(x() * x() + y() * y()));
        }
    };

    /* non-member math functions (also inlined for an attempt at some speed) */
    template <class T>
    inline constexpr coord2<T> operator+(const coord2<T> p1, const coord2<T>& p2)
    {
        return { p1.x() + p2.x(), p1.y() + p2.y() };
    }
    template <class T>
    inline constexpr coord2<T> operator-(const coord2<T>& p1, const coord2<T>& p2)
    {
        return { p1.x() - p2.x(), p1.y() - p2.y() };
    }
    template <class T, typename U>
    inline constexpr coord2<T> operator*(const coord2<T>& p1, U d)
    {
        return { p1.x() * d, p1.y() * d };
    }
    template <class T, typename U>
    inline constexpr coord2<T> operator*(U d, const coord2<T>& p1)
    {
        return { p1.x() * d, p1.y() * d };
    }
    template <class T, typename U>
    inline constexpr coord2<T> operator/(coord2<T> p1, U d)
    {
        if constexpr (
            std::is_same<T, double>::value || std::is_same<T, float>::value
            || std::is_same<U, double>::value || std::is_same<U, float>::value)
        {
            auto v = T(1) / d;
            return { v * p1.x(), v * p1.y() };
        }
        else
        {
            return { p1.x() / d, p1.y() / d };
        }
    }

    template <class T>
    inline coord2<T> operator*(coord2<T> p1, const coord2<T>& p2)
    {
        return { p1.x() * p2.x(), p1.y() * p2.y() };
    }

    /* Unary minus */
    template <class T>
    inline constexpr coord2<T> operator-(const coord2<T>& p1)
    {
        return { -p1.x(), -p1.y() };
    }

    template <class T>
    inline constexpr T dotprod(const coord2<T>& c1, const coord2<T>& c2)
    {
        return (c1.x() * c2.x()) + (c1.y() * c2.y());
    }

    template <class T>
    inline constexpr T length(const coord2<T>& c)
    {
        return sqrt(dotprod(c, c));
    }

    template <class T>
    inline constexpr coord2<T> unit(const coord2<T>& c)
    {
        return c / c.length();
    }

    template <class T>
    std::ostream& operator<<(std::ostream& o, const coord2<T>& c)
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
