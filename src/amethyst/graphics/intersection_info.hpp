#pragma once

#include "amethyst/graphics/conditional_value.hpp"
#include "amethyst/general/defaulted_value.hpp"
#include "amethyst/graphics/shapes/shape_fwd.hpp"
#include <vector>
#include "amethyst/math/vector3.hpp"
#include "amethyst/math/coord2.hpp"
#include "amethyst/math/onb.hpp"
#include "amethyst/math/unit_line3.hpp"

namespace amethyst
{
    /**
     *
     * Information about an intersection (point-based intersection).
     *
     * @author Kevin Harris <kpharris@users.sourceforge.net>
     * @version $Revision: 1.8 $
     *
     */
    template <class T, typename color_type>
    class intersection_info
    {
    protected:
        typedef conditional_value<const shape<T,color_type>*, ibool> conditional_shape;
        typedef conditional_value<T, ibool> conditional_scalar;
        typedef conditional_value<point3<T>, ibool> conditional_point;

        typedef conditional_value<vector3<T>, ibool> conditional_vector3;
        typedef conditional_value<std::vector<const shape<T,color_type>*>, ibool> conditional_shape_vector;
        typedef conditional_value<coord2<T>, ibool> conditional_coord2;
        typedef conditional_value<onb<T>, ibool> conditional_onb;
        typedef conditional_value<unit_line3<T>, ibool> conditional_line;
        typedef conditional_value<std::vector<intersection_info<T,color_type>>, ibool> conditional_intersection_list;

        conditional_shape shape_hit;
        conditional_scalar first_hit_distance;
        conditional_point point_of_hit;
        conditional_line ray;
        conditional_coord2 uv_of_hit;
        conditional_onb onb_of_hit;
        conditional_vector3 normal;
        conditional_shape_vector all_containers;
        conditional_intersection_list all_intersections;

    public:
        const shape<T,color_type>* get_shape() const { return *shape_hit; }
        T get_first_distance() const { return *first_hit_distance; }
        point3<T> get_first_point() const { return *point_of_hit; }
        unit_line3<T> get_ray() const { return *ray; }
        coord2<T> get_uv() const { return *uv_of_hit; }
        onb<T> get_onb() const { return *onb_of_hit; }
        vector3<T> get_normal() const { return *normal; }
        const std::vector<const shape<T,color_type>*>& get_containers() const { return *all_containers; }
        std::vector<intersection_info<T,color_type>> get_all_intersections() const { return *all_intersections; }

        void set_shape(const shape<T,color_type>* s) { shape_hit = conditional_shape(s, true); }
        void set_first_distance(T dist) { first_hit_distance = conditional_scalar(dist, true); }
        void set_first_point(const point3<T>& p) { point_of_hit = conditional_point(p, true); }
        void set_ray(const unit_line3<T>& r) { ray = conditional_line(r, true); }
        void set_uv(const coord2<T>& c) { uv_of_hit = conditional_coord2(c, true); }
        void set_onb(const onb<T>& o) { onb_of_hit = conditional_onb(o, true); }
        void set_normal(const vector3<T>& n) { normal = conditional_vector3(n, true); }
        void set_containers(const std::vector<const shape<T,color_type>*>& cv) { all_containers = conditional_shape_vector(cv, true); }
        void set_all_intersections(const std::vector<intersection_info<T,color_type>>& intersections) { all_intersections = conditional_intersection_list(intersections, true); }

        void append_container(const shape<T,color_type>* s)
        {
            if (!all_containers.do_test())
            {
                all_containers = conditional_shape_vector(std::vector<const shape<T,color_type>*>(), true);
            }
            all_containers->push_back(s);
        }
        void append_intersection(const intersection_info<T,color_type>& info)
        {
            if (!all_intersections.do_test())
            {
                all_intersections = conditional_intersection_list(std::vector<intersection_info<T,color_type>>(), true);
            }
            all_intersections->push_back(info);
        }

        bool have_shape() const { return shape_hit.do_test(); }
        bool have_distance() const { return first_hit_distance.do_test(); }
        bool have_point() const { return point_of_hit.do_test(); }
        bool have_ray() const { return ray.do_test(); }
        bool have_uv() const { return uv_of_hit.do_test(); }
        bool have_onb() const { return onb_of_hit.do_test(); }
        bool have_normal() const { return normal.do_test(); }
        bool have_containers() const { return all_containers.do_test(); }
        bool have_multiple_intersections() const { return all_intersections.do_test(); }

        // Calculate any important values that are missing, based on other values contained herein.
        void calculate_missing();

    }; // class intersection_info

    template <typename T, typename color_type>
    void intersection_info<T,color_type>::calculate_missing()
    {
        // If we have the ray, and the distance, the point should be available.
        if (ray.do_test() && first_hit_distance.do_test() && !point_of_hit.do_test())
        {
            point_of_hit = conditional_point( ray->point_at(*first_hit_distance), true);
        }

        // FIXME! Add more.
    }

}
