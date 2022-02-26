#pragma once

#include "surface_texture.hpp"
#include "graphics/image_loader.hpp"
#include "math/interval.hpp"
#include "math/vector2.hpp"

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
            m_scale.set(T(m_image->get_width()), T(m_image->get_height()));
        }
        image_texture(std::shared_ptr<image_type> img, image_mapping_type type = image_mapping_type::once)
            : m_image(img)
            , m_scale(m_image->get_width(), m_image->get_height())
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
            m_image = std::make_shared<image_type>(convert_image<typename color_type::number_type>(load_image_with_stb(filename)));
            // auto loader = getImageLoader<T, color_type>(filename);
            // m_image = std::make_shared<image_type>(loader->input(filename));
        }

        std::string name() const override
        {
            return "image_texture";
        }

        color_type get_color_at_location(const coord2<T>& coord, const vector3<T>& normal) const override
        {
            if (m_type == image_mapping_type::once)
            {
                if ((coord.x() > 1) || (coord.x() < 0) || (coord.y() > 1) || (coord.y() < 0))
                {
                    return color_type(0, 0, 0);
                }
            }

            coord2<T> clipped{ fmod(coord.x(), T(1.0)), fmod(coord.y(), T(1)) };

            if (clipped.x() < 0)
            {
                clipped.x() += T(1);
            }
            if (clipped.y() < 0)
            {
                clipped.y() += T(1);
            }

            // Now clipped is in [0,1]^2

            coord2<T> wh{ T(m_image->get_width()), T(m_image->get_height()) };

            auto scaled = clipped * wh;

            // FIXME! Interpolate and make this less ugly.
            auto pix = (*m_image)(
                size_t(scaled.x()),
                size_t((m_image->get_height() - 1) - scaled.y())
                );
            return pix;
        }

        std::string internal_members(const std::string& indentation, bool prefix_with_classname = false) const override
        {
            std::string retval;
            std::string internal_tagging = indentation;

            if (prefix_with_classname)
            {
                internal_tagging += image_texture::name() + "::";
            }

            retval += internal_tagging + string_format("image=[%1,%2]\n", m_image->get_width(), m_image->get_height());
            retval += internal_tagging + "type=" + ((m_type == image_mapping_type::once) ? "once" : "repeated");
            retval += internal_tagging + string_format("scale=%1\n", inspect(m_scale));

            return retval;
        }

    private:
        std::shared_ptr<image_type> m_image;
        image_mapping_type m_type;
        vector2<T> m_scale;
    };
}