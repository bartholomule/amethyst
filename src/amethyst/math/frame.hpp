#pragma once
/*
   frame.hpp -- A class to represent a complete coordinate system.

   Defining a frame with the u,v,w axii (think of them as x,y,z renamed
   to avoid any confusion) and the origin, will allow transformation
   from the global frame into that frame (transform), and from
   that frame into the global frame (inverse_transform).

   All transformations are simple rotations and translations.  No skew,
   scale, or any other such nasties can be performed.
 */

/*
   Revision History:
   18May2001 Created this file.
   18Sep2003 Renamed this file--(frame.hpp instead of frame.h), added
   the namespace, removed the matrix (as I don't think it is
   needed for simple coordinate system changes).  Also most
   things were rewritten to use vectors and points instead of
   coords.
 */

#include "onb.hpp"
#include <ostream>

namespace amethyst
{
    template <class T>
    class frame
    {
    public:
        frame(const point3<T>& origin, const vector3<T>& w)
            : m_basis(w)
            , m_origin(origin)
        {
        }
        frame(const point3<T>& origin, const vector3<T>& a, const vector3<T>& b)
            : m_basis(a, b)
            , m_origin(origin)
        {
        }

        frame(const point3<T>& origin, const vector3<T>& a, const vector3<T>& b, const vector3<T>& c)
            : m_basis(a, b, c)
            , m_origin(origin)
        {
        }

        frame(const frame<T>& f) = default;
        frame<T>& operator=(const frame<T>& f) = default;

        vector3<T> u() const { return m_basis.get_u(); }
        vector3<T> v() const { return m_basis.get_v(); }
        vector3<T> w() const { return m_basis.get_w(); }
        point3<T> o() const { return m_origin; }
        onb<T>& basis() { return m_basis; }
        point3<T>& origin() { return m_origin; }
        const onb<T>& basis() const { return m_basis; }
        constexpr const point3<T>& origin() const { return m_origin; }

        //
        // Transfrom from external frame INTO this frame.
        //
        vector3<T> transform(const vector3<T>& v) const
        {
            return m_basis.into_onb(v);
        }

        point3<T> transform(const point3<T>& p) const
        {
            vector3<T> v = p - origin();
            return m_basis.into_onb(point3<T>(v.x(), v.y(), v.z()));
        }

        coord3<T> transform(const coord3<T>& c) const
        {
            coord3<T> c2(m_origin.x(), m_origin.y(), m_origin.z());
            return m_basis.into_onb(c - c2);
        }

        //
        // Transform OUT OF this frame into the 'global' frame.
        //
        vector3<T> inverse_transform(const vector3<T>& v) const
        {
            return m_basis.outof_onb(v);
        }

        point3<T> inverse_transform(const point3<T>& p) const
        {
            return m_basis.outof_onb(p) +
                   vector3<T>(m_origin.x(), m_origin.y(), m_origin.z());
        }

        coord3<T> inverse_transform(const coord3<T>& c) const
        {
            coord3<T> c2(m_origin.x(), m_origin.y(), m_origin.z());
            return m_basis.outof_onb(c) + c2;
        }

        //
        // Transfrom from external frame INTO this frame, treating the
        // triplet as requested.
        //
        coord3<T> transform_as_point(const coord3<T>& c)
        {
            point3<T> p = transform(point3<T>(c));
            return coord3<T>(p.x(), p.y(), p.z());
        }

        coord3<T> transform_as_vector(const coord3<T>& c)
        {
            vector3<T> p = transform(vector3<T>(c));
            return coord3<T>(p.x(), p.y(), p.z());
        }

        coord3<T> transform_as_normal(const coord3<T>& c)
        {
            // Normals would only be rotated.  They are NOT scaled, skewed, or
            // anything else.  All we have to do is make sure it is a unit
            // vector.
            return unit(transform_as_vector(c));
        }

        // Transform into the frame (from the global).
        template <class vector_type>
        vector_type transform_in(const vector_type& v)
        {
            return transform(v);
        }

        // Transform from the frame to the global.
        template <class vector_type>
        vector_type transform_out(const vector_type& v)
        {
            return inverse_transform(v);
        }

    private:
        onb<T> m_basis;
        point3<T> m_origin;
    };


    //
    // This function will give the single frame which allows the
    // complete transformation of both frames (applied as inner first,
    // then outer).
    //
    template <class T>
    frame<T> get_combined_frame(const frame<T>& outer_frame,
                                const frame<T>& inner_frame)
    {
        return frame<T>(outer_frame.inverse_transform(inner_frame.o()),
                        outer_frame.inverse_transform(inner_frame.u()),
                        outer_frame.inverse_transform(inner_frame.v()),
                        outer_frame.inverse_transform(inner_frame.w()));
    } // get_combined_frame(frame, frame)

    template <class T>
    std::ostream& operator<<(std::ostream& o, const frame<T>& f)
    {
        o << "frame( " << f.o() << "," << f.basis() << " )";
        return o;
    }

    template <typename T>
    std::string inspect(const frame<T>& f)
    {
        return "frame(" + inspect(f.o()) + "," + inspect(f.basis()) + "}";
    }
}