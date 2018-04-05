#pragma once

#include <memory>
#include "amethyst/graphics/noise.hpp"
#include "amethyst/graphics/solid_texture.hpp"
#include "amethyst/graphics/interpolated_value.hpp"

namespace amethyst
{
    /**
     *
     * A texture that is based on solid noise.
     *
     * @author Kevin Harris <kpharris@users.sourceforge.net>
     * @version $Revision: 1.1 $
     *
     */
    template <typename T, typename color_type>
    class noise_texture : public solid_texture<T, color_type>
    {
    public:
        typedef std::shared_ptr<interpolated_value<T, color_type>> interp_type;

        noise_texture(T scale = T(1), const std::shared_ptr<Random<T>>& rnd = std::shared_ptr<Random<T>>(new default_random<T>()));
        noise_texture(const color_type& c0, const color_type& c1, T scale = T(1), const std::shared_ptr<Random<T>>& rnd = std::shared_ptr<Random<T>>(new default_random<T>()));
        noise_texture(const interp_type& colors, T scale = T(1), const std::shared_ptr<Random<T>>& rnd = std::shared_ptr<Random<T>>(new default_random<T>()));

        virtual ~noise_texture() = default;

        color_type get_color_at_point(const point3<T>& location) const override;

        virtual std::string internal_members(const std::string& indentation, bool prefix_with_classname = false) const override;
        virtual std::string name() const override { return "noise_texture"; }
    private:
        T m_scale;
        noise<T> m_noise;
        interp_type m_colors;
    };

    template <typename T, typename color_type>
    noise_texture<T, color_type>::noise_texture(T scale, const std::shared_ptr<Random<T>>& rnd)
        : noise_texture(
            create_interpolation<T, color_type>(color_type(0.8, 0.0, 0.0), color_type(0.0, 0.0, 0.8)),
            scale, rnd
        )
    {
    }

    template <typename T, typename color_type>
    noise_texture<T, color_type>::noise_texture(const color_type& c0, const color_type& c1, T scale, const std::shared_ptr<Random<T>>& rnd)
        : noise_texture(create_interpolation<T, color_type>(c0, c1), scale, rnd)
    {
    }

    template <typename T, typename color_type>
    noise_texture<T, color_type>::noise_texture(const typename noise_texture<T, color_type>::interp_type& colors, T scale, const std::shared_ptr<Random<T>>& rnd)
        : solid_texture<T, color_type>()
        , m_scale(scale)
        , m_noise(rnd)
        , m_colors(colors)
    {
    }

    template <typename T, typename color_type>
    std::string noise_texture<T, color_type>::internal_members(const std::string& indentation, bool prefix_with_classname) const
    {
        std::string retval;
        std::string internal_tagging = indentation;

        if (prefix_with_classname)
        {
            internal_tagging += noise_texture<T, color_type>::name() + "::";
        }

        retval += internal_tagging + string_format("scale=%1\n", m_scale);

        if (m_colors)
        {
            retval += internal_tagging + string_format("colors=%1\n", m_colors->to_string(indentation, "  "));
        }
        else
        {
            retval += internal_tagging + "colors=NULL\n";
        }

        return retval;
    }

    template <typename T, typename color_type>
    color_type noise_texture<T, color_type>::get_color_at_point(const point3<T>& location) const
    {
        coord3<T> v(location.x() * m_scale, location.y() * m_scale, location.z() * m_scale);
        T noisy_value = (m_noise.value(v) + T(1)) / T(2);

        if (m_colors)
        {
            return m_colors->interpolate(noisy_value);
        }
        return color_type(0);
    }
}
