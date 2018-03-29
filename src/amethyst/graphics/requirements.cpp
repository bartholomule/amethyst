/*
 * $Id: requirements.cpp,v 1.1 2004/05/17 07:17:04 kpharris Exp $
 *
 * Part of "Amethyst" -- A playground for graphics development.
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

#include "requirements.hpp"

namespace amethyst
{

    intersection_requirements::intersection_requirements() :
        my_requirements(intersection_capabilities::NONE)
    {

    } // ~intersection_requirements()


    //---------------------------------------
    // Destructor for class intersection_requirements
    //---------------------------------------
    intersection_requirements::~intersection_requirements()
    {

    } // ~intersection_requirements()

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

} // namespace amethyst
