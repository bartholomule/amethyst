#pragma once

#include "amethyst/general/string_format.hpp"
#include "amethyst/general/defines.hpp"
#include "amethyst/math/point3.hpp"
#include "amethyst/math/unit_line3.hpp"
#include "amethyst/graphics/intersection_info.hpp"
#include <limits>

namespace amethyst
{
    /**
     *
     * A full set of parameters required for a ray.  This is used for
     * intersection tests, texture calculation, and possibly other things.
     *
     * @author Kevin Harris <kpharris@users.sourceforge.net>
     * @version $Revision: 1.2 $
     *
     */
    template <class T>
    class ray_parameters
    {
    public:
        ray_parameters() = default;

        ray_parameters(unit_line3<T>& l, T time = 0, T ior = 1, T contribution = 1, long max_depth = AMETHYST_DEPTH_MAX, long current_depth = 0)
            : line(l)
            , time_ray_was_fired(time)
            , current_ior(ior)
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

        T get_ior() const { return current_ior; }
        void set_ior(T ior) { current_ior = ior; }

        T get_contribution() const { return effective_contribution; }
        void set_contribution(T value) { effective_contribution = value; }

        long get_max_depth() const { return depth_max; }
        void set_max_depth(long val) { depth_max = val; }

        long get_current_depth() const { return depth; }
        void set_current_depth(long val) { depth = val; }

        // Perform a perfect reflection.
        bool perfect_reflection(const intersection_info<T>& info, ray_parameters<T>& results) const;
        // Perform a perfect refraction
        bool perfect_refraction(const intersection_info<T>& info, T ior, ray_parameters<T>& results) const;

        std::string to_string() const;

    private:
        // Makes sure a normal is available, calculates (or just sets) the distance
        // and intersection point.
        bool get_parameters_for_next_ray(const intersection_info<T>& info, T& distance, point3<T>& point) const;

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
        T current_ior = 1;
        // used in determining how far a ray should go (starts out as T(1),
        // decreases with each transmission/reflection)
        T effective_contribution = 1;
        // Specifies hwo deep a ray is allowed to go.
        long depth_max = AMETHYST_DEPTH_MAX;
        // The current depth of the ray
        long depth = 0;

        // Future additions:
        // vector3<T> polarization_direction; // full vector
        // or
        // T polarization_angle; // angle [0-2PI)
        //
        // Lighting method enums, etc.
    };

    template <class T>
    bool ray_parameters<T>::get_parameters_for_next_ray(const intersection_info<T>& info,
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
            distance = info.get_distance();
        }
        else
        {
            distance = length(info.get_point() - line.origin());
        }

        if (info.have_point())
        {
            point = info.get_point();
        }
        else
        {
            point = line.point_at(distance);
        }
        return true;
    }

    // Perform a perfect reflection.
    template <class T>
    bool ray_parameters<T>::perfect_reflection(const intersection_info<T>& info, ray_parameters<T>& results) const
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

        T cos = dotprod(line.direction(), normal);
        // If the ray is going the same direction as the normal for the object which
        // was just hit, flip the cosine, so that the reflected ray will not travel
        // through the object, rather, it should be reflected backwards.
        if (cos > 0)
        {
            cos = -cos;
        }
        vector3<T> new_direction = unit(line.direction() - 2 * cos * normal);

        // Checkme! Should the range be limited like this?
        results.set_line(unit_line3<T>(point, new_direction, interval<T>(line.limits.begin() + distance,
                                                                         line.limits.end() - distance)));
        return true;
    }

    // Perform a perfect refraction
    template <class T>
    bool ray_parameters<T>::perfect_refraction(const intersection_info<T>& info, T ior, ray_parameters<T>& results) const
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

        T cos = dotprod(line.direction(), normal);
        T ratio = current_ior / ior;
        T rad = 1 - (ratio * ratio) * (1 - cos * cos);

        if (rad >= 0)
        {
            vector3<T> new_direction = unit(ratio * (line.direction() - cos * normal) -
                                            (normal * sqrt(rad)));

            // Checkme! Should the range be limited like this?
            results.set_line(unit_line3<T>(point, new_direction, interval<T>(line.limits.begin() + distance,
                                                                             line.limits.end() - distance)));
            return true;
        }
        else
        {
            return false;
        }
    }

    template <class T>
    std::string ray_parameters<T>::to_string() const
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

    template <class T>
    std::ostream& operator<<(std::ostream& o, const ray_parameters<T>& p)
    {
        return o << p.to_string();
    }
}
