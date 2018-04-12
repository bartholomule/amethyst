#pragma once
#include "amethyst/graphics/capabilities.hpp"

namespace amethyst
{
    /**
     *
     * Requirements for an intersection.
     *
     * @author Kevin Harris <kpharris@users.sourceforge.net>
     * @version $Revision: 1.2 $
     *
     */
    class intersection_requirements
    {

    public:
        intersection_requirements() = default;
        virtual ~intersection_requirements() = default;

        // Specifies if only the first hit is required, so that large aggregates
        // need only find the first hit, and not search for the closest.
        bool needs_first_only() const
        {
            return !!(my_requirements & intersection_capabilities::HIT_FIRST);
        }

        // Specifies if all hit points on the object are required, if set to false,
        // only the closest is required.
        bool needs_all_hits() const
        {
            return !!(my_requirements & intersection_capabilities::HIT_ALL);
        }
        // Specifies if the normal at the point of intersection is required.
        bool needs_normal() const
        {
            return !!(my_requirements & intersection_capabilities::NORMAL_CALCULATION);
        }
        // Specifies if all object pointers, including that of the containing
        // objects are required.
        bool needs_containers() const
        {
            return !!(my_requirements & intersection_capabilities::HIT_CONTAINERS);
        }
        // Specifies if a uv coordinate on the surface is required.
        bool needs_uv() const
        {
            return !!(my_requirements & intersection_capabilities::UV_CALCULATION);
        }
        // Specifies if a local coordinate system (at the point of the hit) is
        // required.
        bool needs_local_coord_system() const
        {
            return !!(my_requirements & intersection_capabilities::LOCAL_SYSTEM_CALCULATION);
        }

        void force_first_only(bool val)
        {
            if (val)
            {
                my_requirements |= intersection_capabilities::HIT_FIRST;
                my_requirements &= ~intersection_capabilities::HIT_ALL;
            }
            else
            {
                my_requirements &= ~intersection_capabilities::HIT_FIRST;
                my_requirements |= intersection_capabilities::HIT_ALL;
            }
        }
        void force_all_hits(bool val)
        {
            if (val)
            {
                my_requirements &= ~intersection_capabilities::HIT_FIRST;
                my_requirements |= intersection_capabilities::HIT_ALL;
            }
            else
            {
                my_requirements |= intersection_capabilities::HIT_FIRST;
                my_requirements &= ~intersection_capabilities::HIT_ALL;
            }
        }
        void force_normal(bool val)
        {
            if (val)
            {
                my_requirements |= intersection_capabilities::NORMAL_CALCULATION;
            }
            else
            {
                my_requirements &= ~intersection_capabilities::NORMAL_CALCULATION;
            }
        }
        void force_containers(bool val)
        {
            if (val)
            {
                my_requirements |= intersection_capabilities::HIT_CONTAINERS;
            }
            else
            {
                my_requirements &= ~intersection_capabilities::HIT_CONTAINERS;
            }
        }
        void force_uv(bool val)
        {
            if (val)
            {
                my_requirements |= intersection_capabilities::UV_CALCULATION;
            }
            else
            {
                my_requirements &= ~intersection_capabilities::UV_CALCULATION;
            }
        }
        void force_local_coord_system(bool val)
        {
            if (val)
            {
                my_requirements |= intersection_capabilities::LOCAL_SYSTEM_CALCULATION;
            }
            else
            {
                my_requirements &= ~intersection_capabilities::LOCAL_SYSTEM_CALCULATION;
            }
        }

        intersection_capabilities get_matching_capabilities() const;
        bool capabilities_match(intersection_capabilities c) const;

    protected:
        intersection_capabilities my_requirements = intersection_capabilities::NONE;
    };
}
