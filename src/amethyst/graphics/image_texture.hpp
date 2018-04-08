#pragma once

#include "surface_texture.hpp"
#include "image_loader.hpp"
#include "math/interval.hpp"

namespace amethyst
{
    enum class image_mapping_type
    {
        once,
        repeated
    };

    template <typename T, typename color_type>
    class image_texture : public surface_texture<T, color_type>
    {
    public:
        using image_type = raster<color_type>;

        image_texture(const std::string& filename, image_mapping_type type = image_mapping_type::once)
            : m_type(type)
        {
            load_image(filename);
            m_limits_x.set(0, T(m_image->get_width()));
            m_limits_y.set(0, T(m_image->get_height()));
        }
        image_texture(std::shared_ptr<image_type> img, image_mapping_type type = image_mapping_type::once)
            : m_image(img)
            , m_limits_x(0, T(m_image->get_width()))
            , m_limits_y(0, T(m_image->get_height()))
        {
        }
        image_texture(image_type img, image_mapping_type type = image_mapping_type::once)
            : image_texture(std::make_shared<image_type>(std::move(img)), type)
        {
        }
        image_texture(const image_texture&) = default;
        ~image_texture() = default;

        void load_image(const std::string& filename)
        {
            auto loader = getImageLoader<T, color_type>(filename);
            m_image = std::make_shared<image_type>(loader->input(filename));
        }

        std::string name() const override
        {
            return "image_texture";
        }

        color_type get_color_at_location(const coord2<T>& coord, const vector3<T>& normal) const override
        {
            coord2<T> clipped = coord;
            if (m_limits_x.outside(clipped.x()) || m_limits_y.outside(clipped.y()))
            {
                if (m_type == image_mapping_type::once)
                {
                    return color_type(0, 0, 0);
                }
                clipped.set(
                    fmod(clipped.x(), m_limits_x.size()),
                    fmod(clipped.y(), m_limits_y.size())
                );
                if (clipped.x() < 0)
                {
                    clipped.x() += m_limits_x.size();
                }
                if (clipped.y() < 0)
                {
                    clipped.y() += m_limits_y.size();
                }
            }
            // FIXME! Interpolate and make this less ugly.
            return (*m_image)(size_t(clipped.x()), m_image->get_height() - 1 - size_t(clipped.y()));
        }

        std::string internal_members(const std::string& indentation, bool prefix_with_classname = false) const override
        {
            std::string retval;// = texture<T, color_type>::internal_members(indentation, prefix_with_classname);
            std::string internal_tagging = indentation;

            if (prefix_with_classname)
            {
                internal_tagging += image_texture::name() + "::";
            }

            retval += internal_tagging + string_format("image=[%1,%2]\n", m_image->get_width(), m_image->get_height());
            retval += internal_tagging + "type=" + ((m_type == image_mapping_type::once) ? "once" : "repeated");
            retval += internal_tagging + string_format("limits=[%1,%2]\n", inspect(m_limits_x), inspect(m_limits_y));

            return retval;
        }

    private:
        std::shared_ptr<image_type> m_image;
        image_mapping_type m_type;
        interval<T> m_limits_x;
        interval<T> m_limits_y;
    };
}