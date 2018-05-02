#pragma once

#include <algorithm>
#include "../general/template_functions.hpp"
#include "../general/traits.hpp"
#include "quaternion.hpp"

/*
 * The functions in this file take any kind of 3d vector, providing that it supplies 3 things:
 * (1) x(), y(), z() functions (const is ok).
 * (2) [] operator(unsigned) (const is ok)
 * (3) a typedef for its base type as a public member named 'base'.
 */

namespace amethyst
{
    namespace traits
    {
        DEFINE_TRAITS_FOR_MEMBER(has_function_x, x, const&);
        DEFINE_TRAITS_FOR_MEMBER(has_function_y, y, const&);
        DEFINE_TRAITS_FOR_MEMBER(has_function_z, z, const&);

        template <typename T>
        struct vector_size
        {
            static const size_t value =
                size_t(has_function_x<T>::value) +
                size_t(has_function_y<T>::value) +
                size_t(has_function_z<T>::value);
        };
    }

    template <size_t s>
    struct vector_ops
    {
    };

    template<>
    struct vector_ops<3>
    {
        template <typename vector_type>
        static inline vector_type invert(const vector_type& v)
        {
            return vector_type(1 / v.x(), 1 / v.y(), 1 / v.z());
        }

        template <typename vector_type>
        static inline typename vector_type::base average(const vector_type& v)
        {
            return (v.x() + v.y() + v.z()) / 3;
        }

        template <typename vector_type>
        static inline typename vector_type::base max(const vector_type& v)
        {
            return std::max(v.x(), std::max(v.y(), v.z()));
        }

        template <typename vector_type>
        static inline vector_type abs_vector(const vector_type& v)
        {
            return vector_type(my_abs(v.x()), my_abs(v.y()), my_abs(v.z()));
        }

        template <typename vector_type>
        static inline unsigned min_abs_index(const vector_type& v)
        {
            if (my_abs(v.x()) <= my_abs(v.y()))
            {
                if (my_abs(v.x()) <= my_abs(v.z()))
                {
                    return 0;
                }
                return 2;
            }
            if (my_abs(v.y()) <= my_abs(v.z()))
            {
                return 1;
            }
            return 2;
        }
    };

    template<>
    struct vector_ops<2>
    {
        template <typename vector_type>
        static inline vector_type invert(const vector_type& v)
        {
            return vector_type(1 / v.x(), 1 / v.y());
        }

        template<typename vector_type>
        static inline typename vector_type::base average(const vector_type& v)
        {
            return (v.x() + v.y()) / 2;
        }

        template <typename vector_type>
        static inline typename vector_type::base max(const vector_type& v)
        {
            return std::max(v.x(), v.y());
        }

        template <typename vector_type>
        static inline vector_type abs_vector(const vector_type& v)
        {
            return vector_type(my_abs(v.x()), my_abs(v.y()));
        }

        template <typename vector_type>
        static inline unsigned min_abs_index(const vector_type& v)
        {
            if (my_abs(v.x()) <= my_abs(v.y()))
            {
                return 0;
            }
            return 1;
        }
    };

#define FORWARD_VECTOR_FUNCTION(NAME) \
    template <typename vector_type> \
    inline auto NAME(const vector_type& v) -> decltype(vector_ops<traits::vector_size<vector_type>::value>::NAME(v)) \
    { \
        return vector_ops<traits::vector_size<vector_type>::value>::NAME(v); \
    }

    FORWARD_VECTOR_FUNCTION(invert);
    FORWARD_VECTOR_FUNCTION(average);
    FORWARD_VECTOR_FUNCTION(max);
    FORWARD_VECTOR_FUNCTION(abs_vector);
    FORWARD_VECTOR_FUNCTION(min_abs_index);

    // Return a vector perpendicular to the one supplied.
    // CHECKME! Is this correct at all?  It seems to work in simple cases.
    template <typename vector_type>
    inline vector_type perp_vector(const vector_type& v)
    {
        constexpr size_t s = traits::vector_size<vector_type>::value;
        static_assert(s == 2 || s == 3, "Not implemented for vectors of this size");

        if constexpr(s == 2)
        {
            return { v.y(), -v.x() };
        }
        else
        {
            vector_type temp = unit(v);
            unsigned zero_index = min_abs_index(temp);
            if (zero_index == 0)
            {
                return unit(vector_type(0, v.z(), -v.y()));
            }
            else if (zero_index == 1)
            {
                return unit(vector_type(v.z(), 0, -v.x()));
            }
            else
            {
                return unit(vector_type(v.y(), -v.x(), 0));
            }
        }
    }

    template <typename vector_type> inline bool magnitude_greater(const vector_type& v1, const vector_type& v2)
    {
        return dotprod(v1, v1) > dotprod(v2, v2);
    }

    template <typename vector_type> inline bool magnitude_less(const vector_type& v1, const vector_type& v2)
    {
        return dotprod(v1, v1) < dotprod(v2, v2);
    }

    // Finds the best planar projection (axii), given the normal of the plane.
    template <typename vector_type>
    void best_planar_projection(const vector_type& normal, unsigned& axis1, unsigned& axis2)
    {
        if ((my_abs(normal.z()) > my_abs(normal.y())) &&
            (my_abs(normal.z()) > my_abs(normal.x())))
        {
            // X-Y projection best (z-major normal)
            axis1 = 0; axis2 = 1;
        }
        else if (my_abs(normal.y()) > my_abs(normal.x()))
        {
            // X-Z projection is best (Y-Major normal)
            axis1 = 0; axis2 = 2;
        }
        else
        {
            // Only case left: X-Major normal (y-z projection)
            axis1 = 1; axis2 = 2;
        }
    } // best_planar_projection(const coord3&, unsigned&, unsigned&)


    template <typename vector_type>
    void calculate_perpendicular_vectors(const vector_type& normal, vector_type& u, vector_type& v)
    {
        // Given the "best planar projection", this will say which index should
        // be modified.  Note that the diagonal (0,0) (1,1) (2,2) won't ever be
        // used (can't be returned by best_planar_projection)
        const unsigned index_to_modify[3][3] = { { 0, 0, 2 }, { 0, 0, 1 }, { 2, 1, 0 } };

        unsigned i1, i2;
        best_planar_projection(normal, i1, i2);
        unsigned i3 = index_to_modify[i1][i2];

        vector_type vn = normal;
        vn[i3] = vn[i3] + 1;

        v = unit(crossprod(normal, vn));
        u = unit(crossprod(v, normal));
    }

    template <typename vector_type, typename T>
    vector_type rotated_from(const vector_type& v, T angle)
    {
        auto p = perp_vector(v);
        auto v2 = quaternion<typename vector_type::base>::rotate(v, angle, p);
        return { v2.x(), v2.y(), v2.z() };
    }

    template <typename vector_type, typename T>
    vector_type rotated_from_degrees(const vector_type& v, T angle)
    {
        return rotated_from(v, angle * M_PI / 180);
    }
}