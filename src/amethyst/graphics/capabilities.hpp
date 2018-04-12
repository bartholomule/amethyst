#pragma once

#include "general/enum.hpp"
#include <string>

namespace amethyst
{
#define MAKE_CAPABILITY_DECLARATION(name) \
    enum class name; \
    bool operator!(name); \
    MAKE_ORABLE_ENUM_CONVERTABLE_DECLARATIONS(name)

    MAKE_CAPABILITY_DECLARATION(intersection_capabilities)
    {
        NONE = 0,
        HIT_FIRST = 1,
        HIT_ALL = 2,
        HIT_CONTAINERS = 4,
        NORMAL_CALCULATION = 8,
        UV_CALCULATION = 16,
        LOCAL_SYSTEM_CALCULATION = 32,
        TIME_SAMPLING = 64,
        ALL = TIME_SAMPLING * 2 - 1
    };

    MAKE_CAPABILITY_DECLARATION(object_capabilities)
    {
        NONE = 0,
        NOT_FINITE = 1,
        BOUNDABLE = 2,
        MOVABLE = 4,
        POLYGONIZATION = 8,
        CONTAINER = 16,
        IMPLICIT = 32,
        SIMPLE = 64, // A simple shape with fast intersection test (sphere, plane, etc).
        ALL = 2 * SIMPLE - 1
    };

    MAKE_CAPABILITY_DECLARATION(material_capabilities)
    {
        NONE = 0,
        DIFFUSE = 1, // has a color
        EMISSIVE = 2, // emits light
        REFLECTIVE = 4, // specular reflection
        TRANSMISSIVE = 8, // implies refraction of some kind
        ALL = 2 * TRANSMISSIVE - 1
    };
}
