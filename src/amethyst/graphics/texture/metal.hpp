#pragma once

#include "amethyst/graphics/texture/texture.hpp"

namespace amethyst
{
    template <class T, class color_type>
    class metal : public texture<T, color_type>
    {
    public:
        metal(const color_type& albedo, T fuzz = 0.1)
            : m_albedo(albedo)
            , m_fuzz(std::max(std::min(fuzz, 1.0), 0.0))
        {
        }
        virtual ~metal() = default;

        using texture<T, color_type>::get_color;
        bool get_color(const point3<T>& location, const coord2<T>& coord, const vector3<T>& normal, color_type& c) const override
        {
            c = { 0, 0, 0 };
            return true;
        }

        bool scatter_ray(const ray_parameters<T, color_type>& ray, const intersection_info<T, color_type>& intersection, ray_parameters<T, color_type>& reflected, color_type& attenuation) const override
        {
            if (ray.perfect_reflection(intersection, reflected))
            {
                const auto& p = intersection.get_first_point();
                const auto& n = intersection.get_normal();
                auto target = p + n + m_fuzz * vector3<T>(m_sampler.next_sample());
                reflected.set_line({ p, target - p, reflected.get_line().limits() });
                attenuation = m_albedo;
                return true;
            }
            return false;
        }

        std::string internal_members(const std::string& indentation, bool prefix_with_classname = false) const override
        {
            std::string retval;
            std::string internal_tagging = indentation;

            if (prefix_with_classname)
            {
                internal_tagging += metal<T, color_type>::name() + "::";
            }

            retval += internal_tagging + string_format("albedo=%1\n", m_albedo);
            retval += internal_tagging + string_format("fuzz=%1\n", m_fuzz);

            return retval;
        }
        std::string name() const override { return "metal"; }

        material_capabilities get_material_capabilties() const override
        {
            return material_capabilities::DIFFUSE | material_capabilities::REFLECTIVE;
        }

    private:
        color_type m_albedo;
        T m_fuzz;
        mutable sphere_sample_3d<T> m_sampler;
   };
}