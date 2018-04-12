#pragma once

#include "amethyst/graphics/texture/texture.hpp"

namespace amethyst
{
    /**
     *
     * A simple texture (constant color).
     *
     * @author Kevin Harris <kpharris@users.sourceforge.net>
     * @version $Revision: 1.2 $
     *
     */
    template <class T, class color_type>
    class simple_texture : public texture<T, color_type>
    {
    public:
        simple_texture(const color_type& color) : m_color(color) { }
        virtual ~simple_texture() = default;

        color_type get_color(const point3<T>& location, const coord2<T>& coord, const vector3<T>& normal) const override
        {
            return m_color;
        }

        std::string internal_members(const std::string& indentation, bool prefix_with_classname = false) const override
        {
            std::string retval;
            std::string internal_tagging = indentation;

            if (prefix_with_classname)
            {
                internal_tagging += simple_texture<T, color_type>::name() + "::";
            }

            retval += internal_tagging + string_format("color=%1\n", m_color);

            return retval;
        }
        std::string name() const override { return "simple_texture"; }

    private:
        color_type m_color;
    };
}