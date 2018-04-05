#pragma once

#include "point2.hpp"
#include "vector2.hpp"
#include "interval.hpp"

#include <limits>

namespace amethyst
{

    /**
    *
    * A generic base for 2d and 3d lines.
    *
    * @author Kevin Harris <kpharris@users.sourceforge.net>
    * @version $Revision: 1.5 $
    *
    */
    template <typename T, typename point_type, typename vector_type>
    class line_base
    {
    public:
        // -inf -> inf
        static constexpr interval<T> infinite_interval()
        {
            return interval<T>{ std::numeric_limits<T>::lowest(), std::numeric_limits<T>::max() };
        }
        // 0 -> 1
        static constexpr interval<T> segment_one()
        {
            return interval<T>{ std::numeric_limits<T>::min(), T(1) - std::numeric_limits<T>::epsilon() };
        }

        line_base() = default;
        line_base(const point_type& o, const vector_type& v, const interval<T>& limits = infinite_interval());
        line_base(const point_type& p1, const point_type& p2);
        virtual ~line_base() = default;
        line_base(const line_base& old) = default;
        line_base& operator=(const line_base& old) = default;

        constexpr point_type o() const { return line_origin; }
        constexpr point_type origin() const { return line_origin; }
        constexpr vector_type v() const { return line_direction; }
        constexpr vector_type direction() const { return line_direction; }

        point_type minimum() const { return line_origin + line_limits.begin() * direction(); }
        point_type maximum() const { return line_origin + line_limits.end() * direction(); }
        point_type point_at(const T& t) const { return line_origin + direction() * t; }

        bool infinite() const { return infinite_limits; }
        interval<T> limits() const { return line_limits; }
        bool inside(T t) const { return line_limits.inside(t); }
    protected:
        point_type line_origin;
        vector_type line_direction;
        interval<T> line_limits = infinite_interval();
        bool infinite_limits = true;
    };

    template <typename T, typename point_type, typename vector_type>
    line_base<T,point_type,vector_type>::line_base(const point_type& o, const vector_type& v, const interval<T>& limits)
        : line_origin(o)
        , line_direction(v)
        , line_limits(limits)
    {
        // Yes, I know that floating-point comparisons of this sort is usually bad,
        // but if they set it to something other than the default, this should
        // fail, so this wouldn't be infinite.
        infinite_limits = ((line_limits.begin() <= std::numeric_limits<T>::lowest()) &&
            (line_limits.end() >= std::numeric_limits<T>::max()));

    }
    template <typename T, typename point_type, typename vector_type>
    line_base<T,point_type,vector_type>::line_base(const point_type& p1, const point_type& p2)
        : line_base(p1, p2 - p1, segment_one())
    {
    }

    template <typename T, typename point_type, typename vector_type>
    std::ostream& operator<<(std::ostream& o, const line_base<T,point_type,vector_type>& l)
    {
        o << "|" << l.o() << ", " << l.v() << ", " << l.limits() << "|";
        return o;
    }

    template <typename T, typename point_type, typename vector_type>
    std::string inspect(const line_base<T,point_type,vector_type>& l)
    {
        return "|" + inspect(l.o()) + ", " + inspect(l.v()) + ", " + l.limits() + "|";
    }
}