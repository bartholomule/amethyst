#pragma once

#include "amethyst/graphics/texture/texture.hpp"

namespace amethyst
{
    template <class T, class color_type>
    class dielectric : public texture<T, color_type>
    {
    public:
        dielectric(T ior = 1.0, color_type attenuation = { 0.9, 0.9, 0.9 })
            : m_ior(ior)
            , m_attenuation(attenuation)
        {
        }
        virtual ~dielectric() = default;


        bool scatter_ray(const ray_parameters<T, color_type>& ray, const intersection_info<T, color_type>& intersection, ray_parameters<T, color_type>& refracted, color_type& attenuation) const override
        {
            if (ray.perfect_refraction(intersection, m_ior, refracted))
            {
                attenuation = m_attenuation;
                return true;
            }
            else if (ray.perfect_reflection(intersection, refracted))
            {
                attenuation = m_attenuation;
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
                internal_tagging += dielectric<T, color_type>::name() + "::";
            }

            retval += internal_tagging + string_format("ior=%1\n", m_ior);
            retval += internal_tagging + string_format("attenuation=%1\n", m_attenuation);
            retval += internal_tagging + string_format("capabilities=%1\n", get_material_capabilties());

            return retval;
        }
        std::string name() const override { return "dielectric"; }

        material_capabilities get_material_capabilties() const override
        {
            return material_capabilities::TRANSMISSIVE;
        }

    private:
        T m_ior;
        color_type m_attenuation = {};
    };
}