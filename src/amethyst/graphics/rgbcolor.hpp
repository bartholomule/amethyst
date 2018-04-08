#pragma once
#include "math/coord3.hpp"
#include <iostream>

namespace amethyst
{
    /**
     *
     * A generic class to represent RGB colors.  Note that only built-in (or
     * native) types (char, float, etc), are usable in this class.  The design of
     * this class was chosen so that array accesses ([]) and direct accesses
     * (.N()) would be almost the same speed.  Also, this class *may* be able to
     * be used in an array and reinterpreted as an array of the base type.
     *
     * For example:
     *<pre>
     *@@ rgbcolor<unsigned char> foo[100];
     *@@ unsigned char* foo_reinterpreted = reinterpret_cast<char*>(foo);
     *@@ ...
     *</pre>
     *
     * The opposite should also be possible.  In either case, BE CAREFUL to make
     * sure that the correct type of destructor would be called (especially with
     * memory allocated by 'new').
     *
     * Whatver changes or additions are made in the future, NO virtual functions
     * should be added, as they would likely break the ability to reinterpret the
     * class as an array of three ints.
     *
     * @author Kevin Harris <kpharris@users.sourceforge.net>
     * @version $Revision: 1.7 $
     *
     */
    template <class T>
    class rgbcolor : private coord3<T>
    {
    public:
        typedef T number_type;
        using parent = coord3<T>;

        rgbcolor() = default;
        constexpr rgbcolor(T a) : parent(a, a, a) { }
        constexpr rgbcolor(T r, T g, T b) : parent(r, g, b) { }
        /** Destructor -- NON VIRTUAL! (does nothing) */
        ~rgbcolor() = default;
        rgbcolor(const rgbcolor& old) = default;
        rgbcolor& operator= (const rgbcolor& old) = default;

        // Set the r, g, and b components
        inline void set_r(T r) { parent::x() = r; }
        inline void set_g(T g) { parent::y() = g; }
        inline void set_b(T b) { parent::z() = b; }
        constexpr inline T r() const { return parent::x(); }
        constexpr inline T g() const { return parent::y(); }
        constexpr inline T b() const { return parent::z(); }

        /** Get the element specified.  No bounds checking is performed */
        inline T& operator[](unsigned index) { return parent::operator[](index); }
        constexpr inline T operator[](unsigned index) const { return parent::operator[](index); }

        /** Set the components to the given values */
        void set(T r, T g, T b) { parent::set(r, g, b); }

        template <class U>
        rgbcolor& operator*=(U factor) { parent::operator*=(factor); return *this; }
        template <class U>
        rgbcolor& operator/=(U factor) { parent::operator/=(factor); return *this; }
        rgbcolor& operator+=(const rgbcolor& r) { parent::operator+=(r); return *this; }
        rgbcolor& operator-=(const rgbcolor& r) { parent::operator-=(r); return *this; }
    };

    /** Multiplication of a color by a factor (non-member, general) */
    template <class T, class U>
    rgbcolor<T> operator*(U factor, const rgbcolor<T>& r)
    {
        return { T(factor * r.r()), T(factor * r.g()), T(factor  * r.b()) };
    }

    /** Multiplication of a color by a factor (non-member, general) */
    template <class T, class U>
    rgbcolor<T> operator*(const rgbcolor<T>& r, U factor)
    {
        return { T(factor * r.r()), T(factor * r.g()), T(factor  * r.b()) };
    }

    template <typename T>
    rgbcolor<T> operator*(const rgbcolor<T>& l, const rgbcolor<T>& r)
    {
        return { l.r() * r.r(), l.g() * r.g(), l.b() * r.b() };
    }

    template <class T, class U>
    constexpr rgbcolor<T> operator/(const rgbcolor<T>& r, U factor)
    {
        U f = U(1) / factor;
        return { T(f * r.r()), T(f * r.g()), T(f * r.b()) };
    }

    /** Addition of two colors. */
    template <class T>
    rgbcolor<T> operator+(const rgbcolor<T>& c1, const rgbcolor<T>& c2)
    {
        return { c1.r() + c2.r(), c1.g() + c2.g(), c1.b() + c2.b() };
    }

    /** Subtraction of two colors. */
    template <class T>
    rgbcolor<T> operator-(const rgbcolor<T>& c1, const rgbcolor<T>& c2)
    {
        return { c1.r() - c2.r(), c1.g() - c2.g(), c1.b() - c2.b() };
    }

    /** Unary negation of a color. */
    template <class T>
    rgbcolor<T> operator-(const rgbcolor<T>& c)
    {
        return { -c.r(), -c.g(), -c.b() };
    }

    template <class T>
    std::ostream& operator<< (std::ostream& o, const rgbcolor<T>& c)
    {
        o << "[- " << c.r() << "," << c.g() << "," << c.b() << " -]";
        return o;
    }

    template <typename T>
    std::string inspect(const rgbcolor<T>& c)
    {
        return "[- " + inspect(c.r()) + "," + inspect(c.g()) + "," + inspect(c.b()) + " -]";
    }
}