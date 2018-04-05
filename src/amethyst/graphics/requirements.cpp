#include "requirements.hpp"

namespace amethyst
{
    intersection_capabilities intersection_requirements::get_matching_capabilities() const
    {
        intersection_capabilities caps = intersection_capabilities::NONE;

        if (needs_normal())
        {
            caps |= intersection_capabilities::NORMAL_CALCULATION;
        }
        if (needs_uv())
        {
            caps |= intersection_capabilities::UV_CALCULATION;
        }
        if (needs_local_coord_system())
        {
            caps |= intersection_capabilities::LOCAL_SYSTEM_CALCULATION;
        }

        return caps;
    }

    bool intersection_requirements::capabilities_match(intersection_capabilities c) const
    {
        intersection_capabilities req_caps = get_matching_capabilities();

        return (req_caps & c) == req_caps;
    }
}
