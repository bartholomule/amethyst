#pragma once

#include "amethyst/graphics/texture/texture.hpp"

namespace amethyst
{
    template <class T, class color_type>
    class lambertian : public texture<T, color_type>
    {
    public:
        lambertian(const color_type& albedo) : m_albedo(albedo) { }
        virtual ~lambertian() = default;

        bool get_color(const point3<T>& location, const coord2<T>& coord, const vector3<T>& normal, color_type& color) const override
        {
            color = { 0,0,0 };
            return true;
        }


        bool scatter_ray(const ray_parameters<T,color_type>& ray, const intersection_info<T,color_type>& intersection, ray_parameters<T,color_type>& reflected, color_type& attenuation) const override
        {
            if (ray.perfect_reflection(intersection, reflected))
            {
                const auto& p = intersection.get_first_point();
                const auto& n = intersection.get_normal();
                auto target = p + n + vector3<T>(m_sampler.next_sample());
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
                internal_tagging += lambertian<T, color_type>::name() + "::";
            }

            retval += internal_tagging + string_format("albedo=%1\n", m_albedo);

            return retval;
        }
        std::string name() const override { return "lambertian"; }

        material_capabilities get_material_capabilties() const override
        {
            return material_capabilities::DIFFUSE | material_capabilities::REFLECTIVE;
        }

    private:
        color_type m_albedo;
        mutable sphere_sample_3d<T> m_sampler;
    };
}