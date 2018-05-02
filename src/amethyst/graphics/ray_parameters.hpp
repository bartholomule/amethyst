#pragma once

#include "amethyst/general/string_format.hpp"
#include "amethyst/general/defines.hpp"
#include "amethyst/general/reversable.hpp"
#include "amethyst/math/point3.hpp"
#include "amethyst/math/unit_line3.hpp"
#include "amethyst/graphics/intersection_info.hpp"
#include <limits>

namespace amethyst
{
    // Given a unit vector, reflect it over a unit vector normal.
    template <typename T>
    bool reflect(const vector3<T>& line, const vector3<T>& normal, vector3<T>& result)
    {
        T cos = dotprod(line, normal);
        result = unit(line - 2 * cos * normal);
        return true;
    }

    template <typename T>
    bool critical_angle(T ni, T nt, T& result)
    {
        // sin(theta_t) = ni / nt * sin(theta_i)
        // The critical angle happens where (ni/nt) * sin(theta_i) > 1
        // So, theta_i is > asin(nt / ni)

        if (ni <= nt)
        {
            return false;
        }

        result = asin(nt / ni);
        if (result < 0)
        {
            result += 2 * M_PI;
        }
        return true;
    }

    template <typename T>
    T critical_angle(T ni, T nt)
    {
        T result;
        if (critical_angle(ni, nt, result))
        {
            return result;
        }
        if constexpr(std::numeric_limits<T>::has_infinity)
        {
            return std::numeric_limits<T>::infinity();
        }
        else
        {
            return std::numeric_limits<T>::max();
        }
    }


    // Given a unit vector and current/entering IORs, refract over a unit vector normal.
    template <typename T>
    bool refract(const vector3<T>& line, vector3<T> normal, T current_ior, T ior, vector3<T>& result, bool& entering)
    {
        T cos = dotprod(line, normal);
        T ni_over_nt = current_ior / ior;

        entering = cos <= 0;

        if (!entering)
        {
            // Leaving the object.  Flip the ratio.
            ni_over_nt = 1 / ni_over_nt;
            normal = -normal;
        }

        T radical = 1 - (ni_over_nt * ni_over_nt) * (1 - cos * cos);

        if (radical < 0)
        {
            return false;
        }

        result = unit(ni_over_nt * (line - cos * normal) - (normal * sqrt(radical)));
        return true;
    }

    /**
     *
     * A full set of parameters required for a ray.  This is used for
     * intersection tests, texture calculation, and possibly other things.
     *
     * @author Kevin Harris <kpharris@users.sourceforge.net>
     * @version $Revision: 1.2 $
     *
     */
    template <class T, typename color_type>
    class ray_parameters
    {
    public:
        ray_parameters() = default;

        ray_parameters(unit_line3<T>& l, T time = 0, T ior = 1, T contribution = 1, long max_depth = AMETHYST_DEPTH_MAX, long current_depth = 0)
            : line(l)
            , time_ray_was_fired(time)
            , effective_contribution(1)
            , depth_max(max_depth)
            , depth(current_depth)
        {
        }

        virtual ~ray_parameters() = default;

        // Line operations.
        const unit_line3<T>& get_line() const { return line; }
        void set_line(const unit_line3<T>& l) { line = l; }

        // Time operations.
        T get_time() const { return time_ray_was_fired; }
        void set_time(T t) { time_ray_was_fired = t; }

        T get_ior() const { return current_ior().second; }

        T get_scalar_contribution() const
        {
            // FIXME! Not functional with other color types.
            const auto& e = effective_contribution;
            return sqrt(e[0] * e[0] + e[1] * e[1] + e[2] * e[2]);
        }
        color_type get_contribution() const { return effective_contribution; }
        void set_contribution(color_type value) { effective_contribution = value; }

        long get_max_depth() const { return depth_max; }
        void set_max_depth(long val) { depth_max = val; }

        long get_current_depth() const { return depth; }
        void set_current_depth(long val) { depth = val; }

        // Perform a perfect reflection.
        bool perfect_reflection(const intersection_info<T,color_type>& info, ray_parameters<T,color_type>& results) const;
        // Perform a perfect refraction
        bool perfect_refraction(const intersection_info<T,color_type>& info, T ior, ray_parameters<T,color_type>& results) const;

        std::string to_string() const;

        using ior_type = std::pair<const shape<T, color_type>*, T>;
        ior_type current_ior() const
        {
            if (!active_iors.empty())
            {
                return active_iors.back();
            }
            return { nullptr, T(1.0) };
        }
        const std::vector<ior_type>& all_iors() const { return active_iors; }

        bool reflected = false;
        bool refracted = false;

    private:
        // Makes sure a normal is available, calculates (or just sets) the distance
        // and intersection point.
        bool get_parameters_for_next_ray(const intersection_info<T,color_type>& info, T& distance, point3<T>& point) const;

        //
        // Items that shapes may care about.
        //

        // The line of the ray (includes origin, direction, and range)
        unit_line3<T> line;
        // The time value of the ray.
        T time_ray_was_fired = 0;

        //
        // Items that are ignored by shapes (will be used by materials).
        //

        // The IOR of the current medium.
        std::vector<ior_type> active_iors;
        // used in determining how far a ray should go (starts out as T(1),
        // decreases with each transmission/reflection)
        color_type effective_contribution = { 1, 1, 1 };
        // Specifies hwo deep a ray is allowed to go.
        long depth_max = AMETHYST_DEPTH_MAX;
        // The current depth of the ray
        long depth = 0;

        bool entered(const shape<T, color_type>* s) const
        {
            for (const auto& e : reverse(active_iors))
            {
                if (e.first == s)
                {
                    return true;
                }
            }
            return false;
        }

        void enter_object(const shape<T, color_type>* obj, T ior)
        {
            // std::cout << "Entering object with ior " << ior << std::endl;
            active_iors.emplace_back(obj, ior);
        }

        void leave_object(const shape<T,color_type>* obj, T ior)
        {
            auto iter = active_iors.begin();
            for (; iter != active_iors.end(); ++iter)
            {
                if (iter->first == obj)
                {
                    break;
                }
            }
            if (iter != active_iors.end())
            {
                // std::cout << "Leaving object with ior " << ior << std::endl;
                active_iors.erase(iter);
            }
            else
            {
                // The object has its normal reversed, or we entered from the wrong side.
                // CHECKME! Do we really want this?
                enter_object(obj, ior);
            }
        }


        // Future additions:
        // vector3<T> polarization_direction; // full vector
        // or
        // T polarization_angle; // angle [0-2PI)
        //
        // Lighting method enums, etc.
    };

    template <typename T, typename color_type>
    bool ray_parameters<T,color_type>::get_parameters_for_next_ray(const intersection_info<T,color_type>& info,
                                                        T& distance, point3<T>& point) const
    {
        if (depth >= depth_max)
        {
            return false;
        }

        if (!info.have_normal())
        {
            return false;
        }

        if (!info.have_distance() || !info.have_point())
        {
            return false;
        }

        if (info.have_distance())
        {
            distance = info.get_first_distance();
        }
        else
        {
            distance = length(info.get_first_point() - line.origin());
        }

        if (info.have_point())
        {
            point = info.get_first_point();
        }
        else
        {
            point = line.point_at(distance);
        }

        return true;
    }

    // Perform a perfect reflection.
    template <typename T, typename color_type>
    bool ray_parameters<T,color_type>::perfect_reflection(const intersection_info<T,color_type>& info, ray_parameters<T,color_type>& results) const
    {
        T distance;
        point3<T> point;

        if (!get_parameters_for_next_ray(info, distance, point))
        {
            return false;
        }
        results = *this;
        results.set_current_depth(depth + 1);

        vector3<T> normal = info.get_normal();
        vector3<T> new_direction;

        if (!reflect(line.direction(), normal, new_direction))
        {
            return false;
        }

        auto limits = line.limits();
        limits.set(std::max(AMETHYST_EPSILON, limits.begin()), limits.end());
        results.set_line(unit_line3<T>(point, new_direction, limits));
        results.reflected = true;
        return true;
    }

    // Perform a perfect refraction
    template <typename T, typename color_type>
    bool ray_parameters<T, color_type>::perfect_refraction(const intersection_info<T, color_type>& info, T ior, ray_parameters<T, color_type>& results) const
    {
        T distance;
        point3<T> point;

        if (!get_parameters_for_next_ray(info, distance, point))
        {
            return false;
        }
        results = *this;
        results.set_current_depth(depth + 1);

        vector3<T> normal = info.get_normal();
        vector3<T> new_direction;
        bool entering = false;
        if (!refract(line.direction(), normal, get_ior(), ior, new_direction, entering))
        {
            return false;
        }

        auto limits = line.limits();
        limits.set(std::max(AMETHYST_EPSILON, limits.begin()), limits.end());
        results.set_line(unit_line3<T>(point, new_direction, limits));

        if (entering)
        {
            results.enter_object(info.get_shape(), ior);
        }
        else
        {
            results.leave_object(info.get_shape(), ior);
        }

        results.refracted = true;
        return true;
    }

    template <typename T, typename color_type>
    std::string ray_parameters<T,color_type>::to_string() const
    {
        std::string indentation = "  ";
        return "ray\n{\n" +
               indentation + string_format("line=%1\n", line) +
               indentation + string_format("time=%1\n", time_ray_was_fired) +
               indentation + string_format("ior=%1\n", current_ior) +
               indentation + string_format("contribution=%1\n", effective_contribution) +
               indentation + string_format("current_depth=%1\n", depth) +
               indentation + string_format("max_depth=%1\n", depth_max) +
               "}\n";
    }

    template <typename T, typename color_type>
    std::ostream& operator<<(std::ostream& o, const ray_parameters<T,color_type>& p)
    {
        return o << p.to_string();
    }
}
