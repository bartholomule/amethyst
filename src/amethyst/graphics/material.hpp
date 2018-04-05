#pragma once
#include "amethyst/graphics/capabilities.hpp"

namespace amethyst
{

    /**
     *
     * This is the base class for a material.
     *
     * @author Kevin Harris <kpharris@users.sourceforge.net>
     * @version $Revision: 1.1 $
     *
     */
    template <typename color_type, typename T>
    class material : public string_dumpable
    {
    public:
        material() = default;
        virtual ~material() = default;

        std::string internal_members(const std::string& indentation, bool prefix_with_classname = false) const override;
        virtual std::string name() const override
        {
            return "material";
        }

        virtual material_capabilities get_material_capabilties() const
        {
            return material_capabilities::NONE;
        }

        // FIXME! Add functions for getting color, transforming for reflection (specular), transmission, etc.

    };

    template <typename color_type, typename T>
    std::string material<color_type, T>::internal_members(const std::string& indentation, bool prefix_with_classname) const
    {
        std::string retval;
        std::string internal_tagging = indentation;

        if (prefix_with_classname)
        {
            internal_tagging += material<color_type, T>::name() + "::";
        }

        retval += indentation + string_format("capabilities=%1\n", get_material_capabilities().to_string());

        return retval;
    }
}