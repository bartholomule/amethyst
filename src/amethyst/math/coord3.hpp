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

namespace amethyst
{
    template <class T>
    class coord3
    {
    private:
        /* class that has 3 Ts stored directly */
        struct coord3_direct
        {
            T x; T y; T z;
        };
        /* class that has 3 Ts stored in an array */
        struct coord3_array
        {
            T coords[3];
        };
        /* union to allow accesses to both indirectly through an array, and directly
           through a name, without adding any extra processing time or space
           requirements */
        union coord3_union
        {
            coord3_union() = default;
            coord3_union(T x, T y, T z)
                : direct{ x, y, z }
            {
            }
            coord3_union(const coord3_union& old)
                : direct{ old.direct.x, old.direct.y, old.direct.z }
            {
            }
            T& operator[](unsigned index) { return array.coords[index]; }
            T operator[](unsigned index) const { return array.coords[index]; }
            coord3_union& operator=(const coord3_union& old)
            {
                direct.x = old.direct.x;
                direct.y = old.direct.y;
                direct.z = old.direct.z;
                return *this;
            }
            coord3_direct direct;
            coord3_array array;
        };

        /* The internal coordinates of this class */
        coord3_union coords;

    public:
        enum COORD_VALUES
        {
            X = 0, Y = 1, Z = 2
        };
        typedef T base;

        coord3() = default;
        explicit coord3(T u) : coords(u, u, u) { }
        coord3(T x, T y, T z) : coords(x, y, z) { }
        coord3(const coord3& old_coord) = default;

        void set(T x, T y, T z)
        {
            coords.direct.x = x;
            coords.direct.y = y;
            coords.direct.z = z;
        }

        /* Accessors */
        T& operator[](unsigned coord_index) { return coords[coord_index]; }
        T operator[](unsigned coord_index) const { return coords[coord_index]; }
        T& x() { return coords.direct.x; }
        T x() const { return coords.direct.x; }
        T& y() { return coords.direct.y; }
        T y() const { return coords.direct.y; }
        T& z() { return coords.direct.z; }
        T z() const { return coords.direct.z; }

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
            return T(sqrt(double((x() * x()) +
                                 (y() * y()) +
                                 (z() * z()))));
        }

    };

    template <class T>
    inline coord3<T>& coord3<T>::operator+=(const coord3<T>& p2)
    {
        coords.direct.x += p2.coords.direct.x;
        coords.direct.y += p2.coords.direct.y;
        coords.direct.z += p2.coords.direct.z;
        return *this;
    }

    template <class T>
    inline coord3<T>& coord3<T>::operator-=(const coord3<T>& p2)
    {
        coords.direct.x -= p2.coords.direct.x;
        coords.direct.y -= p2.coords.direct.y;
        coords.direct.z -= p2.coords.direct.z;
        return *this;
    }

    // Piecewise multiplication...
    template <class T>
    inline coord3<T>& coord3<T>::operator*=(const coord3<T>& p2)
    {
        coords.direct.x *= p2.coords.direct.x;
        coords.direct.y *= p2.coords.direct.y;
        coords.direct.z *= p2.coords.direct.z;
        return *this;
    }

    template <class T>
    inline coord3<T>& coord3<T>::operator*=(T factor)
    {
        coords.direct.x *= factor;
        coords.direct.y *= factor;
        coords.direct.z *= factor;
        return *this;
    }

    template <class T>
    inline coord3<T>& coord3<T>::operator/=(T factor)
    {
        coords.direct.x /= factor;
        coords.direct.y /= factor;
        coords.direct.z /= factor;
        return *this;
    }

    template <class T>
    template <class U>
    inline coord3<T>& coord3<T>::operator*=(U factor)
    {
        coords.direct.x = T(coords.direct.x * factor);
        coords.direct.y = T(coords.direct.y * factor);
        coords.direct.z = T(coords.direct.z * factor);
        return *this;
    }

    template <class T>
    template <class U>
    inline coord3<T>& coord3<T>::operator/=(U factor)
    {
        coords.direct.x = T(coords.direct.x / factor);
        coords.direct.y = T(coords.direct.y / factor);
        coords.direct.z = T(coords.direct.z / factor);
        return *this;
    }

    /* non-member math functions (also inlined for an attempt at some speed) */
    template <class T>
    inline coord3<T> operator+(const coord3<T>& p1, const coord3<T>& p2)
    {
        coord3<T> p(p1); p += p2; return p;
    }

    template <class T>
    inline coord3<T> operator-(const coord3<T>& p1, const coord3<T>& p2)
    {
        coord3<T> p(p1); p -= p2; return p;
    }

    template <class T>
    inline coord3<T> operator*(const coord3<T>& p1, T factor)
    {
        coord3<T> p(p1); p *= factor;  return p;
    }

    template <class T>
    inline coord3<T> operator*(T factor, const coord3<T>& p1)
    {
        coord3<T> p(p1); p *= factor;  return p;
    }

    template <class T, class U>
    inline coord3<T> operator*(const coord3<T>& p1, U factor)
    {
        coord3<T> p(p1); p *= factor;  return p;
    }

    template <class T, class U>
    inline coord3<T> operator*(U factor, const coord3<T>& p1)
    {
        coord3<T> p(p1); p *= factor;  return p;
    }

    template <class T>
    inline coord3<T> operator*(const coord3<T>& p1, const coord3<T>& p2)
    {
        return coord3<T>(p1.x() * p2.x(),
                         p1.y() * p2.y(),
                         p1.z() * p2.z());
    }
    template <class T>
    inline coord3<T> operator/(const coord3<T>& p1, T factor)
    {
        coord3<T> p(p1); p /= factor;  return p;
    }

    template <class T, class U>
    inline coord3<T> operator/(const coord3<T>& p1, U factor)
    {
        coord3<T> p(p1); p /= factor;  return p;
    }

    // Unary minus
    template <class T>
    inline coord3<T> operator-(const coord3<T>& p1)
    {
        return T(-1) * p1;
    }


    /* exports from this header file */
    template <class T>
    inline T dotprod(const coord3<T>& c1, const coord3<T>& c2)
    {
        return (c1.x() * c2.x()) + (c1.y() * c2.y()) + (c1.z() * c2.z());
    }

    template <class T>
    inline coord3<T> crossprod(const coord3<T>& c1, const coord3<T>& c2)
    {
        return coord3<T>((c1.y() * c2.z()) - (c2.y() * c1.z()),
                         (c2.x() * c1.z()) - (c1.x() * c2.z()),
                         (c1.x() * c2.y()) - (c2.x() * c1.y()));
    }

    template <class T>
    inline T length(const coord3<T>& c)
    {
        return c.length();
    }

    template <class T> inline T squared_length(const coord3<T>& c)
    {
        return dotprod(c, c);
    }

    template <class T> inline coord3<T> unit(const coord3<T>& c)
    {
        return c / c.length();
    }

    template <class T>
    std::ostream& operator<< (std::ostream& o, const coord3<T>& c)
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
