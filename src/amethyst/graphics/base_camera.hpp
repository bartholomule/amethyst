#pragma once
#include "amethyst/math/unit_line3.hpp"
#include "amethyst/general/string_format.hpp"
#include "amethyst/general/string_dumpable.hpp"
#include "amethyst/graphics/ray_parameters.hpp"

namespace amethyst
{
    /**
     *
     * A base class for a camera.
     *
     * @author Kevin Harris <kpharris@users.sourceforge.net>
     * @version $Revision: 1.4 $
     *
     */
    template <class T>
    class base_camera : public string_dumpable
    {
    public:
        base_camera() = default;
        base_camera(size_t w, size_t h) : nx(w), ny(h) { }
        virtual ~base_camera() = default;
        base_camera(const base_camera& old) = default;

        /** Assignment operator */
        base_camera& operator=(const base_camera& old) = default;

        /* Width and height of the image in pixels. */
        size_t width() const { return nx; }
        size_t height() const { return ny; }
        void set_width(size_t w) { nx = w; }
        void set_height(size_t h) { ny = h; }

        /* This sample point is in [0,1]^2 */
        virtual ray_parameters<T> get_ray(const coord2<T>& sample_point, T time = 0) const = 0;
        /* px, py are the pixel positions. */
        virtual ray_parameters<T> get_ray(const T& px, const T& py, T time = 0) const = 0;

        std::string internal_members(const std::string& indentation, bool prefix_with_classname = false) const override;

        std::string name() const override { return "base_camera"; }
    private:
        size_t nx = 0;
        size_t ny = 0;
    };

    template <class T>
    std::string base_camera<T>::internal_members(const std::string& indentation, bool prefix_with_classname) const
    {
        std::string retval;

        std::string internal_tagging = indentation;

        if (prefix_with_classname)
        {
            internal_tagging += base_camera<T>::name() + "::";
        }
        retval += internal_tagging + string_format("width=%1\n", nx);
        retval += internal_tagging + string_format("height=%1\n", ny);
        return retval;
    }

    template <typename T>
    using camera_ptr = std::shared_ptr<base_camera<T>>;
}
