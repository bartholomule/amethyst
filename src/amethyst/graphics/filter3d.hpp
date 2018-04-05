#pragma once
/*
  filter3d.hpp

  A set of filters that will take a set of points, and warp them to fit in the
  density pattern of the given filter (so that the image creation function can
  be sampled and averaged at these points to generate a filtered pixel value).
  They may have other applications later.
 */

/*
  Revision history:
  20jan1999 Created this file.
  08Apr2000 Changed the include of coord2 to be in <graph_math>
  25May2001 Changed this class to be templated, moving the members back in here
            from the filter3d.cpp file.
	    Fixed the separated triangle filter to use the width, which was
	    previously obeing ignored.
 */

// FIXME! This class needs quite a bit of testing.

#include <vector>
#include "math/coord2.hpp"

namespace amethyst
{
    template <class T>
    class filter_3d
    {
    public:
        using coord = coord2<T>;

        inline filter_3d() = default;
        virtual ~filter_3d() = default;

        void warp_points(std::vector<coord>& points_to_warp) const
        {
            if (!points_to_warp.empty())
            {
                coord* first = &*points_to_warp.begin();
                return warp_points(first, first + points_to_warp.size());
            }
            return warp_points()
        }

        virtual void warp_points(coord* first, coord* last) const
        {
            // Base class does no warping.
        }
    };

    template <class T>
    class box_filter_3d : public filter_3d<T>
    {
    public:
        inline box_filter_3d(T width = 2)
            : box_width(width)
        {
        }
        virtual ~box_filter_3d() = default;

        void warp_points(coord* first, coord* last) const override;

    private:
        T box_width;
    };

    template <class T>
    class separated_triangle_filter_3d : public filter_3d<T>
    {
    public:

        inline separated_triangle_filter_3d(T width = 2)
            : triangle_width(width)
        {
        }
        virtual ~separated_triangle_filter_3d() = default;

        void warp_points(coord* first, coord* last) const override;

    private:
        T triangle_width;
    };


    template <class T>
    void box_filter_3d<T>::warp_points(filter3d<T>::coord* first, filter3d<T>::coord* last) const
    {
        T half_width = box_width / 2;
        for(; first != last; ++first)
        {
            first->x() = (first->x()*box_width) - half_width;
            first->y() = (first->y()*box_width) - half_width;
        }
    }

    template <class T>
    void separated_triangle_filter_3d<T>::warp_points(filter3d<T>::coord* first, filter3d<T>::coord* last) const
    {
        T half_width = triangle_width / T(2);

        for(; first != last; ++first)
        {
            if (first->x() < 0.5)
                first->x() = (sqrt(2 * first->x()) - 1) * half_width;
            else
                first->x() = (1 - sqrt(2 - 2 * first->x())) * half_width;

            if (first->y() < 0.5)
                first->y() = (sqrt(2 * first->y()) - 1) * half_width;
            else
                first->y() = (1 - sqrt(2 - 2 * first->y())) * half_width;
        }
    }
}