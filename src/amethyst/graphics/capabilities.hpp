/*
 * $Id: capabilities.hpp,v 1.4 2008/06/21 22:25:10 kpharris Exp $
 *
 * Part of "Amethyst" a playground for graphics development
 * Copyright (C) 2004 Kevin Harris
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
 */

#pragma once

#include <string>

namespace amethyst
{

    // This macro defines a wrapper struct for a bitmask enum.  This struct is
    // named with the value supplied for cap_name.  The wrapper struct allows a
    // set of values to be used as an enum and also provide real use of "&",
    // "|", "~".  String conversions are also supplied to make printing of these
    // enums clean and easy.
    //
    // The enum values must include the symbolic names NONE and ALL. These are
    // used for masking, initialization and other operations.  The value of ALL
    // should normally be twice the largest value minus 1 (see use below).
#define CAPABILITIES_DECLARATION(cap_name, ...) \
    struct cap_name \
    { \
        enum capabilities \
        { \
            __VA_ARGS__ \
        }; \
        capabilities my_capabilities; \
 \
        cap_name() : my_capabilities(cap_name::NONE) {} \
        cap_name(capabilities c) : my_capabilities(c) {} \
        std::string to_string() const; \
        std::string to_string_short() const; \
 \
        cap_name& operator|=(const capabilities c); \
        cap_name& operator|=(const cap_name& c); \
        cap_name& operator&=(const capabilities c); \
        cap_name& operator&=(const cap_name& c); \
    }; \
 \
    bool operator&(const cap_name& c1, cap_name::capabilities c2); \
    bool operator&(cap_name::capabilities c1, const cap_name& c2); \
    cap_name operator&(const cap_name& c1, const cap_name& c2); \
    cap_name operator|(const cap_name& c1, const cap_name& c2); \
    cap_name operator~(const cap_name::capabilities& c); \
    cap_name operator~(const cap_name& c); \
 \
    bool operator==(const cap_name& i1, const cap_name& i2); \
    bool operator!=(const cap_name& i1, const cap_name& i2);

    CAPABILITIES_DECLARATION(intersection_capabilities,
                             NONE = 0,
                             HIT_FIRST = 1,
                             HIT_ALL = 2,
                             HIT_CONTAINERS = 4,
                             NORMAL_CALCULATION = 8,
                             UV_CALCULATION = 16,
                             LOCAL_SYSTEM_CALCULATION = 32,
                             TIME_SAMPLING = 64,
                             ALL = TIME_SAMPLING * 2 - 1
                             );

    CAPABILITIES_DECLARATION(object_capabilities,
                             NONE = 0,
                             INFINITE = 1,
                             BOUNDABLE = 2,
                             MOVABLE = 4,
                             POLYGONIZATION = 8,
                             CONTAINER = 16,
                             IMPLICIT = 32,
                             SIMPLE = 64, // A simple shape with fast intersection test (sphere, plane, etc).
                             ALL = 2 * SIMPLE - 1
                             );

    CAPABILITIES_DECLARATION(material_capabilities,
                             NONE = 0,
                             DIFFUSE = 1, // has a color
                             EMISSIVE = 2, // emits light
                             REFLECTIVE = 4, // specular reflection
                             TRANSMISSIVE = 8, // implies refraction of some kind
                             ALL = 2 * TRANSMISSIVE - 1
                             );
}
