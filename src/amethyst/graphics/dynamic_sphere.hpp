#pragma once

#include "amethyst/graphics/shape.hpp"
#include "amethyst/graphics/interpolated_value.hpp"
#include "amethyst/general/defines.hpp"
#include <memory>
#include "amethyst/general/string_format.hpp"

namespace amethyst
{
    /**
     *
     * A dynamic_sphere class.
     *
     * @author Kevin Harris <kpharris@users.sourceforge.net>
     * @version $Revision: 1.2 $
     *
     */
    template <class T>
    class dynamic_sphere : public shape<T>
    {
    public:
        using parent = shape<T>;

        dynamic_sphere() : dynamic_sphere(point3<T>(0, 0, 0), 1) { }
        dynamic_sphere(const point3<T>& c, T rad);

        dynamic_sphere(const point3<T>& c1, T t1,
                       const point3<T>& c2, T t2,
                       T rad1, T tr1,
                       T rad2, T tr2);

        virtual ~dynamic_sphere() = default;
        dynamic_sphere(const dynamic_sphere& old) = default;
        dynamic_sphere& operator=(const dynamic_sphere& old) = default;
        dynamic_sphere(dynamic_sphere&& old)
            : shape<T>(std::move(old))
            , center(std::move(old.center))
            , radius(std::move(old.radius))
        {
        }

        dynamic_sphere& operator=(dynamic_sphere&& old)
        {
            shape<T>::operator=(std::move(old));
            center.swap(old.center);
            radius.swap(old.radius);
            return *this;
        }

        point3<T> get_center(T time) const
        {
            return point3<T>(center->interpolate(time));
        }
        T get_radius(T time) const
        {
            return radius->interpolate(time);
        }

        /** Returns if the given point is inside the dynamic_sphere. */
        bool inside(const point3<T>& p) const override;

        /** Returns if the given dynamic_sphere intersects the dynamic_sphere. */
        bool intersects(const sphere<T>& s) const override;

        /** Returns if the given plane intersects the shape. */
        bool intersects(const plane<T>& p) const override;


        /** Returns if the given line intersects the dynamic_sphere. */
        using parent::intersects_line;
        bool intersects_line(const unit_line3<T>& line,
            intersection_info<T>& intersection,
            const intersection_requirements& requirements) const override;

        /**
         * A quick intersection test.  This will calculate nothing but the
         * distance. This is most useful for shadow tests, and other tests where no
         * textures will be applied.
         */
        bool quick_intersection(const unit_line3<T>& line, T time, T& distance) const override;

        /**
         * Returns if the given ray intersects the shape.
         *
         */
        bool intersects_ray(const ray_parameters<T>& ray,
            intersection_info<T>& intersection,
            const intersection_requirements& requirements = intersection_requirements()) const override;

        std::string internal_members(const std::string& indentation, bool prefix_with_classname = false) const override;

        std::string name() const override { return "dynamic_sphere"; }

        intersection_capabilities get_intersection_capabilities() const override;
        object_capabilities get_object_capabilities() const override;

    private:
        coord2<T> get_uv(const point3<T>& location, T time) const;

        using center_type = interpolation_point<T, coord3<T>>;
        using radius_type = interpolation_point<T, T>;

        std::shared_ptr<interpolated_value<T, coord3<T>>> center;
        std::shared_ptr<interpolated_value<T, T>> radius;
    };

    template <class T>
    dynamic_sphere<T>::dynamic_sphere(const point3<T>& c, T rad)
        : center(create_interpolation<T, coord3<T>>(c, c))
        , radius(create_interpolation<T, T>(rad, rad))
    {
    }

    template <class T>
    dynamic_sphere<T>::dynamic_sphere(const point3<T>& c1, T t1,
        const point3<T>& c2, T t2,
        T rad1, T tr1,
        T rad2, T tr2)
        // FIXME! These two lines should be identical in the output, but they don't do the same thing.  So there is a bug in the interpolation, but the second looks better.
        // : center(create_interpolation<T, coord3<T>>(center_type(t1, c1.getcoord()), center_type(t2, c2.getcoord())))
        : center(create_interpolation<T, coord3<T>>(std::vector<center_type>({ center_type(t1, c1.getcoord()), center_type(t2, c2.getcoord()) })))
        , radius(create_interpolation<T, T>(radius_type(tr1, rad1), radius_type(tr2, rad2)))
    {
    }

    // Returns if the given point is inside the dynamic_sphere.
    template <class T>
    bool dynamic_sphere<T>::inside(const point3<T>& p) const
    {
        // FIXME! Check the entire range

        // The epsilon adjusted radius is (r^2 + 2*r*E + E^2)
        T my_radius = get_radius(T(0));
        return squared_length(p - get_center(T(0))) <
               (my_radius * my_radius +
                2 * my_radius * AMETHYST_EPSILON +
                AMETHYST_EPSILON * AMETHYST_EPSILON);
    }

    // Returns if the given dynamic_sphere intersects the dynamic_sphere.
    template <class T>
    bool dynamic_sphere<T>::intersects(const sphere<T>& s) const
    {
        T my_radius = get_radius(T(0));
        point3<T> my_center = get_center(T(0));

        T combined_radius = s.get_radius() + my_radius + AMETHYST_EPSILON;
        return squared_length(s.get_center() - my_center) <
               (combined_radius * combined_radius);
    }

    // Returns if the given plane intersects the shape.
    template <class T>
    bool dynamic_sphere<T>::intersects(const plane<T>& p) const
    {
        // FIXME! Check the entire range, do something better.
        sphere<T> s(get_center(T(0)),
                    get_radius(T(0)));

        return amethyst::intersects(s, p);
    }

    // Returns if the given line intersects the dynamic_sphere.
    template <class T>
    bool dynamic_sphere<T>::intersects_line(const unit_line3<T>& line,
                                            intersection_info<T>& intersection,
                                            const intersection_requirements& requirements) const
    {
        // FIXME! Check the entire time range, not just 0....
        T hit_distance;
        T time = T(0);
        if (quick_intersection(line, time, hit_distance))
        {
            intersection.set_shape(this);
            intersection.set_first_distance(hit_distance);
            intersection.set_first_point(line.point_at(hit_distance));
            intersection.set_ray(line);

            if (requirements.needs_normal())
            {
                intersection.set_normal(unit(intersection.get_first_point() - get_center(time)));
            }

            if (requirements.needs_uv())
            {
                intersection.set_uv(get_uv(intersection.get_first_point(), time));
            }

            // FIXME! Follow the rest of the requirements
            // FIXME! multiple intersections if needed.
            return true;
        }
        return false;
    }

    /**
     * A quick intersection test.  This will calculate nothing but the
     * distance. This is most useful for shadow tests, and other tests where no
     * textures will be applied.
     */
    template <class T>
    bool dynamic_sphere<T>::quick_intersection(const unit_line3<T>& line,
                                               T time, T& distance) const
    {
        // FIXME! Check the entire range
        T my_radius = get_radius(time);
        point3<T> my_center = get_center(time);

        return quick_sphere_intersection_test(my_center, my_radius, my_radius * my_radius, line, distance);
    }

    template <class T>
    bool dynamic_sphere<T>::intersects_ray(const ray_parameters<T>& ray,
                                           intersection_info<T>& intersection,
                                           const intersection_requirements& requirements) const
    {
        T hit_distance;
        if (quick_intersection(ray.get_line(), ray.get_time(), hit_distance))
        {
            const unit_line3<T>& line = ray.get_line();
            intersection.set_shape(this);
            intersection.set_first_distance(hit_distance);
            intersection.set_first_point(line.point_at(hit_distance));
            intersection.set_ray(line);

            if (requirements.needs_normal())
            {
                intersection.set_normal(unit(intersection.get_first_point() - get_center(ray.get_time())));
            }

            if (requirements.needs_uv())
            {
                intersection.set_uv(get_uv(intersection.get_first_point(), ray.get_time()));
            }

            // FIXME! multiple intersections

            return true;
        }
        return false;
    }

    template <class T>
    std::string dynamic_sphere<T>::internal_members(const std::string& indentation, bool prefix_with_classname) const
    {
        std::string retval;
        std::string internal_tagging = indentation;

        if (prefix_with_classname)
        {
            internal_tagging += dynamic_sphere<T>::name() + "::";
        }

        retval += indentation + string_format("intersection_capabilities=%1\n", get_intersection_capabilities().to_string());
        retval += indentation + string_format("object_capabilities=%1\n", get_object_capabilities().to_string());
        retval += internal_tagging + string_format("center=%1\n", center->to_string(indentation));
        retval += internal_tagging + string_format("radius=%1\n", radius->to_string(indentation));

        return retval;
    }

    template <class T>
    intersection_capabilities dynamic_sphere<T>::get_intersection_capabilities() const
    {
        intersection_capabilities caps = shape<T>::get_intersection_capabilities();

        caps |= intersection_capabilities::HIT_FIRST;
        caps |= intersection_capabilities::HIT_ALL;
        caps |= intersection_capabilities::NORMAL_CALCULATION;
        caps |= intersection_capabilities::UV_CALCULATION;
        //    caps |= intersection_capabilities::LOCAL_SYSTEM_CALCULATION
        return caps;
    }

    template <class T>
    object_capabilities dynamic_sphere<T>::get_object_capabilities() const
    {
        object_capabilities caps = shape<T>::get_object_capabilities();

        caps |= object_capabilities::BOUNDABLE;
        caps |= object_capabilities::SIMPLE;

        return caps;
    }

    template <class T>
    coord2<T> dynamic_sphere<T>::get_uv(const point3<T>& location, T time) const
    {
        vector3<T> point_vector = location - get_center(time);

        T cos_theta = point_vector.y() / get_radius(time);
        // phi = [-pi,pi]
        // theta = [0,pi]
        T theta = acos(cos_theta);
        T phi = atan2(point_vector.z(), point_vector.x());
        T u = (phi + M_PI) / (2 * M_PI);
        T v = theta / M_PI;

        return coord2<T>(u, v);
    }
}
