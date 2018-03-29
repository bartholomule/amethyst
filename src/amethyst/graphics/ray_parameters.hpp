/*
 * $Id: ray_parameters.hpp,v 1.2 2008/06/21 22:25:10 kpharris Exp $
 *
 * Part of "Amethyst" -- A playground for graphics development.
 * Copyright (C) 2004 Kevin Harris
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
 */

#if       !defined(AMETHYST__RAY_PARAMETERS_HPP)
#define            AMETHYST__RAY_PARAMETERS_HPP

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

    private:
        //
        // Items that shapes may care about.
        //

        // The line of the ray (includes origin, direction, and range)
        unit_line3<T> line;
        // The time value of the ray.
        T time_ray_was_fired;

        //
        // Items that are ignored by shapes (will be used by materials).
        //

        // The IOR of the current medium.
        T current_ior;
        // used in determining how far a ray should go (starts out as T(1),
        // decreases with each transmission/reflection)
        T effective_contribution;
        // Specifies hwo deep a ray is allowed to go.
        long depth_max;
        // The current depth of the ray.
        long depth;

        // Future additions:
        // vector3<T> polarization_direction; // full vector
        // or
        // T polarization_angle; // angle [0-2PI)
        //
        // Lighting method enums, etc.

    protected:

    public:
        /** Default constructor */
        ray_parameters();

        ray_parameters(unit_line3<T>& l, T time = 0, T ior = 1, T contribution = 1, long depth_max = AMETHYST_DEPTH_MAX, long depth = 0);

        /** Destructor */
        virtual ~ray_parameters();

        /** Copy constructor: compiler defaults    */ // ray_parameters(const ray_parameters& old);

        /** Assignment operator: compiler defaults */ // ray_parameters& operator= (const ray_parameters& old);

        // Line operations.
        const unit_line3<T>& get_line() const {
            return line;
        }
        void set_line(const unit_line3<T>& l) {
            line = l;
        }

        // Time operations.
        T get_time() const {
            return time_ray_was_fired;
        }
        void set_time(T t) {
            time_ray_was_fired = t;
        }

        T get_ior() const {
            return current_ior;
        }
        void set_ior(T ior) {
            current_ior = ior;
        }

        T get_contribution() const {
            return effective_contribution;
        }
        void set_contribution(T value) {
            effective_contribution = value;
        }

        long get_max_depth() const {
            return depth_max;
        }
        void set_max_depth(long val) {
            depth_max = val;
        }

        long get_current_depth() const {
            return depth;
        }
        void set_current_depth(long val) {
            depth = val;
        }

        // Perform a perfect reflection.
        bool perfect_reflection(const intersection_info<T>& info, ray_parameters<T>& results) const;
        // Perform a perfect refraction
        bool perfect_refraction(const intersection_info<T>& info, T ior, ray_parameters<T>& results) const;

        std::string to_string() const;

    private:
        // Makes sure a normal is available, calculates (or just sets) the distance
        // and intersection point.
        bool get_parameters_for_next_ray(const intersection_info<T>& info,
                                         T& distance, point3<T>& point) const;

    }; // class ray_parameters



    //---------------------------------------------
    // Default constructor for class ray_parameters
    //---------------------------------------------
    template <class T>
    ray_parameters<T>::ray_parameters() :
        line(),
        time_ray_was_fired(0),
        current_ior(1),
        effective_contribution(1),
        depth_max(AMETHYST_DEPTH_MAX),
        depth(0)
    {

    } // ray_parameters()

    template <class T>
    ray_parameters<T>::ray_parameters(unit_line3<T>& l, T time, T ior, T contribution, long dm, long d) :
        line(l),
        time_ray_was_fired(time),
        current_ior(ior),
        effective_contribution(1),
        depth_max(dm),
        depth(d)
    {
    }


    //------------------------------------
    // Destructor for class ray_parameters
    //------------------------------------
    template <class T>
    ray_parameters<T>::~ray_parameters()
    {

    } // ~ray_parameters()

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
        o << p.to_string();
        return o;
    }

} // namespace amethyst


#endif /* !defined(AMETHYST__RAY_PARAMETERS_HPP) */

