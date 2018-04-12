#pragma once

#include "amethyst/graphics/base_camera.hpp"
#include "amethyst/graphics/ray_parameters.hpp"
#include "amethyst/math/frame.hpp"

namespace amethyst
{

    /**
     *
     * A pinhole camera.
     *
     * @author Kevin Harris <kpharris@users.sourceforge.net>
     * @version $Revision: 1.3 $
     *
     */
    template <typename T, typename color_type>
    class pinhole_camera : public base_camera<T,color_type>
    {
    public:
        pinhole_camera();
        pinhole_camera(const point3<T>& eye,
                       const vector3<T>& gaze,
                       const vector3<T>& up,
                       T virtual_screen_width,
                       T virtual_screen_height,
                       T virtual_screen_distance,
                       size_t w,
                       size_t h,
                       const interval<T>& shutter_open_time = interval<T>());

        virtual ~pinhole_camera() = default;
        pinhole_camera(const pinhole_camera&) = default;
        pinhole_camera& operator=(const pinhole_camera&) = default;

        /* FIXME! At some point in the future, these will need to have a more
           complete ray, which includes time, etc. */
        /* This one uses sample positions */
        virtual ray_parameters<T,color_type> get_ray(const coord2<T>& sample_point, T time = 0) const;

        /* This one uses pixel positions */
        virtual ray_parameters<T,color_type> get_ray(const T& px, const T& py, T time = 0) const;

        virtual std::string internal_members(const std::string& indentation, bool prefix_with_classname = false) const;

        virtual std::string name() const {
            return "pinhole_camera";
        }

    private:
        frame<T> viewing_frame;
        coord2<T> ll_corner;
        coord2<T> ur_corner;
        T viewing_distance;
        coord2<T> vscreen_size;
        interval<T> shutter; // Specifies the time the shutter is open.
    };

    //---------------------------------------------
    // Default constructor for class pinhole_camera
    //---------------------------------------------
    template <typename T, typename color_type>
    pinhole_camera<T,color_type>::pinhole_camera() :
        base_camera<T,color_type>(100, 100),
        viewing_frame(point3<T>(0, 0, 0),
                      vector3<T>(1, 0, 0), vector3<T>(0, 1, 0), vector3<T>(0, 0, 1)),
        ll_corner(-0.5, -0.5),
        ur_corner( 0.5, 0.5),
        viewing_distance(1),
        vscreen_size(ur_corner - ll_corner)
    {

    } // pinhole_camera()

    //-------------------------------------------------------
    // fully-specified  constructor for class pinhole_camera
    //-------------------------------------------------------
    template <typename T, typename color_type>
    pinhole_camera<T,color_type>::pinhole_camera(const point3<T>& eye,
                                      const vector3<T>& gaze,
                                      const vector3<T>& up,
                                      T virtual_screen_width,
                                      T virtual_screen_height,
                                      T virtual_screen_distance,
                                      size_t w,
                                      size_t h,
                                      const interval<T>& shutter_open_time) :
        base_camera<T,color_type>(w, h),
        viewing_frame(eye, gaze, up),
        ll_corner(-virtual_screen_width / 2.0, -virtual_screen_height / 2.0),
        ur_corner( virtual_screen_width / 2.0, virtual_screen_height / 2.0),
        viewing_distance(virtual_screen_distance),
        vscreen_size(virtual_screen_width, virtual_screen_height),
        shutter(shutter_open_time)
    {
    } // pinhole_camera()

    template <typename T, typename color_type>
    ray_parameters<T,color_type> pinhole_camera<T,color_type>::get_ray(const coord2<T>& sample_point, T time) const
    {
        // Note: this flips both x and y, so that it looks like y increases as you
        // move up the screen, and x increases towards the right.  This conversion
        // may be faster to do at output time, however, this simplifies things.
        point3<T> view_point((ll_corner.x() + (T(1) - sample_point.x()) * vscreen_size.x()),
                             (ll_corner.y() + (T(1) - sample_point.y()) * vscreen_size.y()),
                             viewing_distance );
        unit_line3<T> line( viewing_frame.origin(),
                            (viewing_frame.inverse_transform(view_point) -
                             viewing_frame.origin()));


        if (!shutter.empty())
        {
            T adjusted_time = shutter.begin() + time * (shutter.end() - shutter.begin());
            //      std::cout << "Adjusted_time(" << time << ")=" << adjusted_time << std::endl;

            return ray_parameters<T,color_type>( line, adjusted_time );
        }
        else
        {
            //      std::cout << "time=" << time << std::endl;
            return ray_parameters<T,color_type>( line, time );
        }
    }

    template <typename T, typename color_type>
    ray_parameters<T,color_type> pinhole_camera<T,color_type>::get_ray(const T& px, const T& py, T time) const
    {
        // Flip x, so that it appears as though x increases to the right.
        T sx = (base_camera<T,color_type>::width() - px) / T(base_camera<T,color_type>::width() - 1);
        // Flip y, so that it appears as though y increases upwards.
        T sy = (base_camera<T,color_type>::height() - py) / T(base_camera<T,color_type>::height() - 1);

        point3<T> view_point((ll_corner.x() + vscreen_size.x() * sx),
                             (ll_corner.y() + vscreen_size.y() * sy),
                             viewing_distance);

        unit_line3<T> line( viewing_frame.origin(),
                            (viewing_frame.inverse_transform(view_point) -
                             viewing_frame.origin()));


        if (!shutter.empty())
        {
            T adjusted_time = shutter.begin() + time * (shutter.end() - shutter.begin());

            return ray_parameters<T,color_type>( line, adjusted_time );
        }
        else
        {
            return ray_parameters<T,color_type>( line, time );
        }
    }

    template <typename T, typename color_type>
    std::string pinhole_camera<T,color_type>::internal_members(const std::string& indentation, bool prefix_with_classname) const
    {
        std::string retval = base_camera<T,color_type>::internal_members(indentation, prefix_with_classname);

        std::string internal_tagging = indentation;
        if (prefix_with_classname)
        {
            internal_tagging += pinhole_camera<T,color_type>::name() + "::";
        }

        retval += internal_tagging + string_format("frame=%1\n", viewing_frame);
        retval += internal_tagging + string_format("dist=%1\n", viewing_distance);
        retval += internal_tagging + string_format("size=%1\n", vscreen_size);
        retval += internal_tagging + string_format("ll corner=%1\n", ll_corner);
        retval += internal_tagging + string_format("ur corner=%1\n", ur_corner);

        return retval;
    }
}
