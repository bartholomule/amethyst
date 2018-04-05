#pragma once
#include "amethyst/graphics/noise.hpp"
#include "amethyst/graphics/solid_texture.hpp"
#include "amethyst/graphics/interpolated_value.hpp"
#include <memory>


namespace amethyst
{
    /**
     *
     * A texture that is based on solid noise.
     *
     * @author Kevin Harris <kpharris@users.sourceforge.net>
     * @version $Revision: 1.2 $
     *
     */
    template <typename T, typename color_type>
    class marble_texture : public solid_texture<T, color_type>
    {
    public:
        typedef std::shared_ptr<interpolated_value<T, color_type>> interp_type;
        marble_texture(T line_width = 0.5, T scale = T(1), int octaves = 8, const std::shared_ptr<Random<T>>& rnd = std::shared_ptr<Random<T>>(new default_random<T>()));
        marble_texture(const color_type& c0, const color_type& c1, const color_type& c2, T line_width = 0.5, T scale = T(1), int octaves = 8, const std::shared_ptr<Random<T>>& rnd = std::shared_ptr<Random<T>>(new default_random<T>()));
        marble_texture(const interp_type& colors, T line_width = 0.5, T scale = T(1), int octaves = 8, const std::shared_ptr<Random<T>>& rnd = std::shared_ptr<Random<T>>(new default_random<T>()));

        virtual ~marble_texture() = default;

        color_type get_color_at_point(const point3<T>& location) const override;

        std::string internal_members(const std::string& indentation, bool prefix_with_classname = false) const override;
        std::string name() const override { return "marble_texture"; }
    private:
        T m_freq;
        T m_scale;
        noise<T> m_noise;
        interp_type m_colors;
        int m_octaves;

    }; // class marble_texture

    template <typename T, typename color_type>
    marble_texture<T, color_type>::marble_texture(T line_width, T scale, int octaves, const std::shared_ptr<Random<T>>& rnd)
        : marble_texture(
            create_interpolation<T, color_type>(color_type(0.06, 0.04, 0.02), color_type(0.4, 0.2, 0.1), color_type(0.8, 0.8, 0.8), 2),
            line_width, scale, octaves, rnd
        )
    {
    }

    template <typename T, typename color_type>
    marble_texture<T, color_type>::marble_texture(const color_type& c0, const color_type& c1, const color_type& c2, T line_width, T scale, int octaves, const std::shared_ptr<Random<T>>& rnd)
        : marble_texture(
            create_interpolation<T, color_type>(c0, c1, c2),
            line_width, scale, octaves, rnd)
    {
    }

    template <typename T, typename color_type>
    marble_texture<T, color_type>::marble_texture(const typename marble_texture<T, color_type>::interp_type& colors, T line_width, T scale, int octaves, const std::shared_ptr<Random<T>>& rnd)
        : solid_texture<T, color_type>()
        , m_freq(M_PI / line_width)
        , m_scale(scale)
        , m_noise(rnd)
        , m_colors(colors)
        , m_octaves(octaves)
    {
    }

    template <typename T, typename color_type>
    std::string marble_texture<T, color_type>::internal_members(const std::string& indentation, bool prefix_with_classname) const
    {
        std::string retval;
        std::string internal_tagging = indentation;

        if (prefix_with_classname)
        {
            internal_tagging += marble_texture<T, color_type>::name() + "::";
        }

        retval += internal_tagging + string_format("line_width=%1 (frequency=%2)\n", M_PI / m_freq, m_freq);
        retval += internal_tagging + string_format("scale=%1\n", m_scale);
        retval += internal_tagging + string_format("octaves=%1\n", m_octaves);

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
    color_type marble_texture<T, color_type>::get_color_at_point(const point3<T>& location) const
    {
        coord3<T> v(location.x() * m_freq, location.y() * m_freq, location.z() * m_freq);
        T turb = m_scale * m_noise.turbulence(v, m_octaves);

        // Pete does an absolute value here, which I've never done on my marble
        // textures before.  It gives nice sharp edges on the low (dark) side.
        T noisy_value = (sin(v.x() + turb) + T(1)) / T(2);
        //    T noisy_value = tfabs(sin(v.x() + turb));

        if (m_colors)
        {
            return m_colors->interpolate(noisy_value);
        }
        return color_type(0);
    }
}
