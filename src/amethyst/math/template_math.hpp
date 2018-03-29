/*
 * $Id: template_math.hpp,v 1.1 2008/06/16 10:17:49 kpharris Exp $
 *
 * Part of "Amethyst" -- A playground for graphics development.
 * Copyright (C) 2008 Kevin Harris
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

#if       !defined(AMETHYST__TEMPLATE_MATH_HPP)
#define            AMETHYST__TEMPLATE_MATH_HPP

namespace amethyst
{
    template <typename T>
    T tfabs(const T& t)
    {
        if (t < 0)
        {
            return -t;
        }
        return t;
    }
} // namespace amethyst

#endif /* !defined(AMETHYST__TEMPLATE_MATH_HPP) */
