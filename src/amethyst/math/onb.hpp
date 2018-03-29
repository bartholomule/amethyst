#pragma once

/*
   onb.hpp -- A class to represent an ortho-normal basis.

   An ortho-normal basis is a set of 3 mutually perpendicular (ortho) vectors
   which are all unit-length (normal).  As such, it can be used for simple
   transformations on vectors, points, and coordinates involving only a single
   rotation.
 */

/*
   Revision History:
   18May2001 Created this file.  Wrote constructors, accessors, and functions to
   convert to/from the ONB.
   18Sep2003 Set a namespace, renamed the file (hpp instead of h), added another
   constructor, changed the internal types to be vector3 instead of
   coord3.
 */

#include "vector3.hpp"
#include "point3.hpp"
#include "vector_utils.hpp"
#include <ostream>

namespace amethyst
{
    template <class T>
    class onb
    {
    public:
        inline onb()
            : m_w(0, 0, 1)
            , m_v(0, 1, 0)
            , m_u(1, 0, 0)
        {
        }

        // An ONB where only the direction of w matters...
        inline onb(const vector3<T>& a)
            : m_w(unit(a))
            , m_v(perp_vector(m_w))
            , m_u(crossprod(m_v, m_w))
        {
        }

        // An ONB where the direction of u,v, and w matter...
        // a and b define a plane, where a is in the direction of w, b is in the
        // direction of v, and u is the normal to this plane.
        inline onb(const vector3<T>& a, const vector3<T>& b)
            : m_w(unit(a))
            , m_v(unit(b))
            , m_u(crossprod(m_v, m_w))
        {
        }

        // An ONB where the direction of u,v, and w are fully specified.
        inline onb(const vector3<T>& a, const vector3<T>& b, const vector3<T>& c)
            : m_w(unit(c))
            , m_v(unit(b))
            , m_u(unit(a))
        {
        }

        onb(const onb<T>& o) = default;
        onb<T>& operator=(const onb<T>& o) = default;

        // Take an external coord, and convert to one for this ONB
        inline coord3<T> into_onb(const coord3<T>& c) const
        {
            vector3<T> cv(c);
            return coord3<T>(dotprod(cv, m_u), dotprod(cv, m_v), dotprod(cv, m_w));
        }

        // Take an external point, and convert to one for this ONB
        inline point3<T> into_onb(const point3<T>& c) const
        {
            vector3<T> cv(c.x(), c.y(), c.z());
            return point3<T>(dotprod(cv, m_u), dotprod(cv, m_v), dotprod(cv, m_w));
        }

        // Take an external vector, and convert to one for this ONB
        inline vector3<T> into_onb(const vector3<T>& c) const
        {
            return vector3<T>(dotprod(c, m_u), dotprod(c, m_v), dotprod(c, m_w));
        }

        // Take an internal coord, and convert to one for a 'global' ONB.
        inline coord3<T> outof_onb(const coord3<T>& c) const
        {
            vector3<T> cv(m_u * c.x() + m_v * c.y() + m_w * c.z());
            return coord3<T>(cv.x(), cv.y(), cv.z());
        }

        // Take an internal point, and convert to one for a 'global' ONB.
        inline point3<T> outof_onb(const point3<T>& c) const
        {
            vector3<T> cv(m_u * c.x() + m_v * c.y() + m_w * c.z());
            return point3<T>(cv.x(), cv.y(), cv.z());
        }

        // Take an internal vector, and convert to one for a 'global' ONB.
        inline vector3<T> outof_onb(const vector3<T>& c) const
        {
            return m_u * c.x() + m_v * c.y() + m_w * c.z();
        }

        inline const vector3<T>& u() const { return m_u; }
        inline const vector3<T>& v() const { return m_v; }
        inline const vector3<T>& w() const { return m_w; }
        inline vector3<T>& u() { return m_u; }
        inline vector3<T>& v() { return m_v; }
        inline vector3<T>& w() { return m_w; }

    private:
        vector3<T> m_w;
        vector3<T> m_v;
        vector3<T> m_u;
    };

    template <class T>
    std::ostream& operator<<(std::ostream& o, const onb<T>& b)
    {
        o << "onb( " << b.u() << ", " << b.v() << ", " << b.w() << ")";
        return o;
    }

    template <typename T>
    std::string inspect(const onb<T>& b)
    {
        return "onb( " + inspect(b.u()) + ", " + inspect(b.v()) + ", " + inspect(b.w()) + ")";
    }
}