/*
 * $Id: string_dumpable.cpp,v 1.2 2008/12/15 17:27:26 kpharris Exp $
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

#include "string_dumpable.hpp"
#include <ostream>

namespace amethyst
{
  string_dumpable::string_dumpable()
  {
  } // string_dumpable()

  string_dumpable::~string_dumpable()
  {
  } // ~string_dumpable()

	std::string string_dumpable::to_string(const std::string& base_indentation,
		 const std::string& level_indentation) const
	{
		 return (
				base_indentation + name() + "\n" +
				base_indentation + "{\n" +
				internal_members(base_indentation + level_indentation, true) +
				base_indentation + "}"
		 );
	}

  std::ostream& operator<<(std::ostream& o, const string_dumpable& s)
  {
    o << s.to_string();
	 return o;
  }
} // namespace amethyst

