#pragma once
/*
 * This file provides classes/functions for interpolating values using various
 * methods.  For now, this includes interpolation among:
 * # Two values
 * # A list of values
 * The values and the parameters at which the values are reached can be
 * speficied, as well as endpoint behavior (stop, continue), interpolation
 * methods (block, linear, cubic).
 *
 * In the future, I hope to be able to have a full spline, where smooth curve
 * fitting is performed in each section (as close to the desired values as
 * possible), and the curve is continuous from one section to another (no sharp
 * changes).
 *
 * For simplicity of use, everything shoule be used through a pointer or
 * reference to the base class (to minimize the number of template parameters
 * required), and the interpolation created from a (cloned, if using a pointer)
 * version of something returned from create_interpolation().
 *
 * For example:
 * typedef vector3<double> val_type;
 * // This creates an interpolation object which varies across [0,1] from
 * // VECTOR1 to VECTOR2.  Endpoint behavior and interpolation method are set
 * // at their defaults (ENDPOINT_STOP, and INERP_CUBIC);
 * interpolated_value<double, val_type>* val = create_interpolation<double,val_type>(VECTOR1,VECTOR2).clone_new()
 *
 * SOME_VECTOR = val->interpolate(SOME_VALUE);
 * ...
 * delete val;
 *
 *
 * To minimize the number of required specializations of interpolation types, a
 * single function is used (basic_interpolation).  To properly interpolate
 * types such as matrices, this must be specialized otherwise strange
 * rotations, scalings, or skews may happen in intermediate matrices.
 */


#include "amethyst/general/string_format.hpp"
#include "amethyst/general/string_dumpable.hpp"
#include "amethyst/general/inspect.hpp"
#include "amethyst/general/enum.hpp"
#include <string>
#include <algorithm>
#include <vector>

namespace amethyst
{
    /**
     * This simple struct is a point along an interpolation curve.
     */
    template <class parametric_type, class interpolation_type>
    struct interpolation_point
    {
        parametric_type parameter;
        interpolation_type value;

        interpolation_point() = default;
        interpolation_point(const parametric_type& p, const interpolation_type& v)
            : parameter(p)
            , value(v)
        {
        }

        // Comparison operators (all defined in terms of < or ==, to reduce the
        // operations needed).
        friend bool operator==(const interpolation_point& p1, const interpolation_point& p2)
        {
            return p1.parameter == p2.parameter;
        }

        friend bool operator!=(const interpolation_point& p1, const interpolation_point& p2)
        {
            return !(p1 == p2);
        }

        friend bool operator<(const interpolation_point& p1, const interpolation_point& p2)
        {
            return p1.parameter < p2.parameter;
        }

        friend bool operator>(const interpolation_point& p1, const interpolation_point& p2)
        {
            return !(p1 < p2) && !(p1 == p2);
        }

        friend bool operator<=(const interpolation_point& p1, const interpolation_point& p2)
        {
            return (p1 < p2) || (p1 == p2);
        }

        friend bool operator>=(const interpolation_point& p1, const interpolation_point& p2)
        {
            return !(p1 < p2);
        }

        friend std::ostream& operator<<(std::ostream& o, const interpolation_point& ip)
        {
            return o << "{ " << ip.parameter << ", " << ip.value << " }";
        }
    };

    template <class parametric_type, class interpolation_type>
    std::string inspect(const interpolation_point<parametric_type, interpolation_type>& p)
    {
        return "{ param=" + inspect(p.parameter) + ", value=" + inspect(p.value) + "}";
    }

    /**
     *
     * This is a class for interpolated values.  The main use of this class will
     * be for parametric-changing parameters (eg. location, radius, etc.)
     *
     * @author Kevin Harris <kpharris@users.sourceforge.net>
     * @version $Revision: 1.7 $
     *
     */
    template <class parametric_type, class interpolation_type>
    class interpolated_value : public string_dumpable
    {
    public:
        interpolated_value() = default;
        virtual ~interpolated_value() = default;

        virtual bool valid_parameter(parametric_type parameter) const = 0;
        virtual interpolation_type interpolate(parametric_type parameter) const = 0;
        virtual interpolated_value<parametric_type, interpolation_type>* clone_new() const = 0;

        virtual std::vector<interpolation_point<parametric_type, interpolation_type>> get_interpolation_points() const = 0;

        std::string name() const override { return "interpolation_base_class"; }

        std::string internal_members(const std::string& indentation, bool prefix_with_classname = false) const override
        {
            return std::string();
        }
    };

    enum class endpoint_action { stop, continue_forever };
    MAKE_ENUM_CONVERTABLE_DECLARATIONS(endpoint_action);


    // This function exists so that only a single function needs to be
    // specialized in order to make complex types (eg. a matrix) usable
    // in all of these interpolation functions even if they require more
    // complicated interpolation methods.
    template <class parametric_type, class interpolation_type>
    inline interpolation_type basic_interpolation(parametric_type adjusted_value,
        const interpolation_type& v1, const interpolation_type& v2)
    {
        return v1 * parametric_type(1 - adjusted_value) +
               v2 * adjusted_value;
    }

    template <class parametric_type, class interpolation_type>
    inline interpolation_type block_interpolate(parametric_type val,
        const interpolation_type& v1, const interpolation_type& v2, endpoint_action behavior)
    {
        return v1;
    }

    template <class parametric_type, class interpolation_type>
    inline interpolation_type linear_interpolate(parametric_type val,
        const interpolation_type& v1, const interpolation_type& v2, endpoint_action behavior = endpoint_action::stop)
    {
        if ((val < 0) && (behavior == endpoint_action::stop))
        {
            return v1;
        }
        if ((val > 1) && (behavior == endpoint_action::stop))
        {
            return v2;
        }

        return basic_interpolation(val, v1, v2);
    }


    namespace helper_functions
    {
        // This function was adapted from Peter Shirley's "Realist Raytracing" second
        // edition.  I have no clue why the given constants were chosen.
        template <class T>
        inline T interpolated_cubic_filter_solve(T t)
        {
            T u = t;
            for (int i = 0; i < 5; ++i)
            {
                u = ((T(11) * t +
                      u * u * (u * (T(8) - u * T(9)) + T(6))) /
                     (T(4) + u * T(12) * (u * (T(1) - u) + T(1))));
            }
            return u;
        }

        // This function was adapted from Peter Shirley's "Realist Raytracing" second
        // edition.  I have no clue why the given constants were chosen.
        template <class T>
        inline T interpolated_cubic_filter(T t)
        {
            const T t1 = T(1.0 / 24.0);
            const T t2 = T(1) - t1;
            if (t < t1)
            {
                return T(pow(24.0 * t, 0.25) - 2.0);
            }
            else if (t < T(0.5))
            {
                return interpolated_cubic_filter_solve(T(24) * (t - t1) / T(11)) - T(1);
            }
            else if (t < t2)
            {
                return T(1) - interpolated_cubic_filter_solve(T(24) * (t2 - t) / T(11));
            }
            else
            {
                return T(2.0 - pow(24.0 * (1.0 - t), 0.25));
            }
        }
    } // end namespace helper_functions

    template <class parametric_type, class interpolation_type>
    inline interpolation_type cubic_interpolate(parametric_type val,
        const interpolation_type& v1, const interpolation_type& v2, endpoint_action behavior)
    {
        // FIXME! This is only minimally correct. It does do the proper curve to the
        // nearest neighbor, but real bicubic interpolation needs to check values
        // farther away.

        if ((val < 0) && (behavior == endpoint_action::stop))
        {
            return v1;
        }
        if ((val > 1) && (behavior == endpoint_action::stop))
        {
            return v2;
        }
        // Filter (warp) the parameter so that linear interpolation will appear to have used
        // the correct values for the interpolation components.
        parametric_type val2 = (3 * val * val - 2 * val * val * val);

        return basic_interpolation(val2, v1, v2);
    }


    template <class parametric_type, class interpolation_type>
    struct interpolation_base
    {
        std::string name() const { return "interpolation_base"; }

        interpolation_type operator()(const parametric_type& param,
            const parametric_type& pmin, const parametric_type& pmax,
            const interpolation_type& vmin, const interpolation_type& vmax,
            endpoint_action b) const
        {
            return block_interpolate(param, vmin, vmax, b);
        }
    };

    template <class parametric_type, class interpolation_type>
    struct linear_interpolation : public interpolation_base<parametric_type, interpolation_type>
    {
        std::string name() const { return "linear_interpolation"; }

        interpolation_type operator()(const parametric_type& param,
            const parametric_type& pmin, const parametric_type& pmax,
            const interpolation_type& vmin, const interpolation_type& vmax,
            endpoint_action b) const
        {
            parametric_type t = (param - pmin) / (pmax - pmin);
            return linear_interpolate(t, vmin, vmax, b);
        }
    };

    template <class parametric_type, class interpolation_type>
    struct cubic_interpolation : public interpolation_base<parametric_type, interpolation_type>
    {
        std::string name() const { return "cubic_interpolation"; }

        interpolation_type operator()(const parametric_type& param,
            const parametric_type& pmin, const parametric_type& pmax,
            const interpolation_type& vmin, const interpolation_type& vmax,
            endpoint_action b) const
        {
            parametric_type t = (param - pmin) / (pmax - pmin);
            return cubic_interpolate(t, vmin, vmax, b);
        }
    };

    template <class parametric_type, class interpolation_type, class interpolation_function>
    struct user_interpolation : public interpolation_base<parametric_type, interpolation_type>
    {
        user_interpolation() = delete;
        user_interpolation(interpolation_function f, std::string fn_name)
            : fun(f), user_function_name(fn_name)
        {
        }

        std::string name() const { return "user_interpolation_function<" + user_function_name + ">"; }

        interpolation_type operator()(const parametric_type& param,
            const parametric_type& pmin, const parametric_type& pmax,
            const interpolation_type& vmin, const interpolation_type& vmax,
            endpoint_action b) const
        {
            parametric_type t = (param - pmin) / (pmax - pmin);
            return fun(t, vmin, vmax, b);
        }

    private:
        interpolation_function fun;
        std::string user_function_name;
    };


    template <class parametric_type, class interpolation_type, class interpolation_function>
    class interpolated_pair : public interpolated_value<parametric_type, interpolation_type>
    {
    public:
        typedef interpolated_pair<parametric_type, interpolation_type, interpolation_function> self_type;
        typedef amethyst::interpolation_point<parametric_type, interpolation_type> interpolation_point;

        interpolated_pair(const interpolation_type& v1, const interpolation_type& v2,
            interpolation_function f = interpolation_function(),
            endpoint_action b = endpoint_action::stop)
            : interpolated_pair(interpolation_point(0, v1), interpolation_point(1, v2), f, b)
        {
        }

        interpolated_pair(const interpolation_point& p1, const interpolation_point& p2,
            interpolation_function f = interpolation_function(),
            endpoint_action b = endpoint_action::stop)
            : ip1(p1)
            , ip2(p2)
            , fun(f)
            , behavior(b)
        {
        }
        // NOTE: copy constructor/assign operator use compiler default.
        virtual ~interpolated_pair() = default;
        virtual self_type* clone_new() const { return new self_type(*this); }

        virtual std::string name() const { return "interpolated_pair<" + fun.name() + ">"; }

        virtual std::string internal_members(const std::string& indentation, bool prefix_with_classname = false) const
        {
            std::string retval = interpolated_value<parametric_type, interpolation_type>::internal_members(indentation, prefix_with_classname);
            std::string internal_tagging = indentation;

            if (prefix_with_classname)
            {
                internal_tagging += self_type::name() + "::";
            }

            retval += internal_tagging + string_format("p1=%1\n", ip1);
            retval += internal_tagging + string_format("p2=%1\n", ip2);
            retval += internal_tagging + string_format("endpoint=%1\n", behavior);

            return retval;
        }

        virtual bool valid_parameter(parametric_type parameter) const
        {
            return (parameter >= ip1.parameter) && (parameter <= ip2.parameter);
        }

        virtual interpolation_type interpolate(parametric_type parameter) const
        {
            return fun((parameter - ip1.parameter) / (ip2.parameter - ip1.parameter),
                       ip1.parameter, ip2.parameter,
                       ip1.value, ip2.value, behavior);
        }

        virtual std::vector<interpolation_point> get_interpolation_points() const
        {
            std::vector<interpolation_point> retval;
            retval.push_back(ip1);
            retval.push_back(ip2);
            return retval;
        }

    private:
        interpolation_point ip1, ip2;
        interpolation_function fun;
        endpoint_action behavior;
    };



    template <class parametric_type, class interpolation_type, class interpolation_function>
    class interpolated_vector : public interpolated_value<parametric_type, interpolation_type>
    {
    public:
        typedef interpolated_vector<parametric_type, interpolation_type, interpolation_function> self_type;
        typedef amethyst::interpolation_point<parametric_type, interpolation_type> interpolation_point;

        interpolated_vector(const interpolation_type& v1, const interpolation_type& v2,
            interpolation_function f = interpolation_function(),
            endpoint_action b = endpoint_action::stop)
            : iv{ { interpolation_point(parametric_type(0), v1), interpolation_point(parametric_type(0), v2) } }
            , fun(f)
            , behavior(b)
        {
        }

        interpolated_vector(const interpolation_point& p1, const interpolation_point& p2,
            interpolation_function f = interpolation_function(),
            endpoint_action b = endpoint_action::stop)
            : iv{ { p1, p2 } }
            , fun(f)
            , behavior(b)
        {
            // Make sure the points are in order.
            std::sort(iv.begin(), iv.end());
        }

        interpolated_vector(std::vector<interpolation_point> v,
            interpolation_function f = interpolation_function(),
            endpoint_action b = endpoint_action::stop)
            : iv(std::move(v))
            , fun(f)
            , behavior(b)
        {
            // Make sure the points are in order.
            std::sort(iv.begin(), iv.end());
        }

        // NOTE: copy constructor/assign operator use compiler default.
        virtual ~interpolated_vector() = default;
        virtual self_type* clone_new() const { return new self_type(*this); }
        virtual std::string name() const { return "interpolated_vector<" + fun.name() + ">"; }

        virtual std::string internal_members(const std::string& indentation, bool prefix_with_classname = false) const
        {
            std::string retval = interpolated_value<parametric_type, interpolation_type>::internal_members(indentation, prefix_with_classname);
            std::string internal_tagging = indentation;

            if (prefix_with_classname)
            {
                internal_tagging += self_type::name() + "::";
            }

            retval += internal_tagging + string_format("v=%1\n", iv);
            retval += internal_tagging + string_format("endpoint=%1\n", behavior);

            return retval;
        }

        virtual bool valid_parameter(parametric_type parameter) const
        {
            if (iv.empty())
            {
                return false;
            }
            parametric_type start = iv.begin()->parameter;
            parametric_type finish = iv.rbegin()->parameter;
            return (parameter >= start) && (parameter <= finish);
        }

        virtual interpolation_type interpolate(parametric_type parameter) const
        {
            using std::cout;
            using std::endl;
            if (iv.size() >= 2)
            {
                size_t i = 0;

                for (; (i < iv.size()); ++i)
                {
                    if (iv[i].parameter > parameter)
                    {
                        break;
                    }
                }


                size_t before = 0, after = 0;
                if ((i < iv.size()) && (i > 0))
                {
                    before = i - 1;
                    after = i;
                }
                else if (i == 0)
                {
                    before = i;
                    after = i + 1;
                }
                else
                {
                    before = iv.size() - 2;
                    after = before + 1;
                }

                parametric_type t = parameter;

                return fun(t,
                    iv[before].parameter, iv[after].parameter,
                    iv[before].value, iv[after].value, behavior);
            }
            else if (!iv.empty())
            {
                return iv.begin()->value;
            }
            else
            {
                return interpolation_type(0);
            }
        }

        std::vector<interpolation_point> get_interpolation_points() const override
        {
            return iv;
        }
    private:
        std::vector<interpolation_point> iv;
        interpolation_function fun;
        endpoint_action behavior;
    };

    enum class interpolation_method { block, linear, cubic };


    // This struct is only here to allow a proper typedef for an interpolation
    // function prototype.
    template <class T, class IT>
    struct it_wrapper
    {
        typedef IT (*user_interpolation_function)(T, const IT&, const IT&, endpoint_action);
    };

    template <class T, class IT>
    user_interpolation<T, IT, typename it_wrapper<T, IT>::user_interpolation_function>
    get_interpolation_function(interpolation_method method = interpolation_method::cubic)
    {
        typedef typename it_wrapper<T, IT>::user_interpolation_function fn_ptr;

        fn_ptr fun;
        std::string fn_name;

        switch (method)
        {
        case interpolation_method::block:
            fun = &block_interpolate<T, IT>;
            fn_name = "block";
            break;
        case interpolation_method::linear:
            fun = &linear_interpolate<T, IT>;
            fn_name = "linear";
            break;
        case interpolation_method::cubic:
            fun = &cubic_interpolate<T, IT>;
            fn_name = "cubic";
            break;
        }

        return user_interpolation<T, IT, fn_ptr>(fun, fn_name);
    }

    template <class T, class IT>
    std::shared_ptr<interpolated_pair<T, IT, user_interpolation<T, IT, typename it_wrapper<T, IT>::user_interpolation_function>>>
    create_interpolation(const interpolation_point<T, IT>& i1,
                         const interpolation_point<T, IT>& i2,
                         endpoint_action b = endpoint_action::stop,
                         interpolation_method method = interpolation_method::cubic)
    {
        typedef typename it_wrapper<T, IT>::user_interpolation_function fn_ptr;

        user_interpolation<T, IT, fn_ptr> interpolation = get_interpolation_function<T, IT>(method);

        return std::make_shared<interpolated_pair<T, IT, user_interpolation<T, IT, fn_ptr>>>(i1, i2, interpolation, b);
    }


    template <class T, class IT>
    std::shared_ptr<interpolated_vector<T, IT, user_interpolation<T, IT, typename it_wrapper<T, IT>::user_interpolation_function>>>
    create_interpolation(const std::vector<interpolation_point<T, IT>>& vec,
                         endpoint_action b = endpoint_action::stop,
                         interpolation_method method = interpolation_method::cubic)
    {
        typedef typename it_wrapper<T, IT>::user_interpolation_function fn_ptr;

        user_interpolation<T, IT, fn_ptr> interpolation = get_interpolation_function<T, IT>(method);

        return std::make_shared<interpolated_vector<T, IT, user_interpolation<T, IT, fn_ptr>>>(vec, interpolation, b);
    }

    template <class T, class IT>
    std::shared_ptr<interpolated_vector<T, IT, user_interpolation<T, IT, typename it_wrapper<T, IT>::user_interpolation_function>>>
    create_interpolation(const IT& i1, const IT& i2, const IT& i3, T exponent = 1,
                         endpoint_action b = endpoint_action::stop,
                         interpolation_method method = interpolation_method::cubic)
    {
        std::vector<interpolation_point<T, IT>> vec
        {
            interpolation_point<T, IT>(T(0.0), i1),
            interpolation_point<T, IT>(pow(T(0.5), exponent), i2),
            interpolation_point<T, IT>(T(1.0), i3)
        };

        return create_interpolation<T, IT>(vec, b, method);
    }
}
