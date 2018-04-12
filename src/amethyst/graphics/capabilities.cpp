#include "capabilities.hpp"

namespace amethyst
{
#define MAKE_CAPABILITY_DEFINITION(name, unmatched_fmt, empty, ...) \
    bool operator!(name n) { return n == empty; } \
    MAKE_ORABLE_ENUM_CONVERTABLE_DEFINITIONS(name, unmatched_fmt, __VA_ARGS__)

    MAKE_CAPABILITY_DEFINITION(intersection_capabilities, "bad intersection capability: %1", intersection_capabilities::NONE,
        { intersection_capabilities::NONE, "None" },
        { intersection_capabilities::HIT_FIRST, "First Object Hit" , "F" },
        { intersection_capabilities::HIT_ALL, "All Objects Hit", "A" },
        { intersection_capabilities::HIT_CONTAINERS, "Containers Hit", "C" },
        { intersection_capabilities::NORMAL_CALCULATION, "Normal Calculation", "N" },
        { intersection_capabilities::UV_CALCULATION, "UV Calculation", "U" },
        { intersection_capabilities::LOCAL_SYSTEM_CALCULATION, "Local Coord System Calculation", "L" },
        { intersection_capabilities::TIME_SAMPLING, "Time Sampling", "T" }
    );

    MAKE_CAPABILITY_DEFINITION(object_capabilities, "bad object capability: %1", object_capabilities::NONE,
        { object_capabilities::NONE, "None" },
        { object_capabilities::NOT_FINITE, "Infinite", "I" },
        { object_capabilities::BOUNDABLE, "Boundable", "B" },
        { object_capabilities::MOVABLE, "Movable", "M" },
        { object_capabilities::POLYGONIZATION, "Polygonization", "P" },
        { object_capabilities::CONTAINER, "Container", "C" },
        { object_capabilities::IMPLICIT, "Implicit Surface", "F" }
    );

    MAKE_CAPABILITY_DEFINITION(material_capabilities, "bad material capability: %1", material_capabilities::NONE,
        { material_capabilities::NONE, "None" },
        { material_capabilities::DIFFUSE, "Diffuse", "D" },
        { material_capabilities::EMISSIVE, "Emissive", "E" },
        { material_capabilities::REFLECTIVE, "Reflective", "R" },
        { material_capabilities::TRANSMISSIVE, "Transmissive", "T" }
    );
}
