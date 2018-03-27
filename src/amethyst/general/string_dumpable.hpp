/*
 * $Id: string_dumpable.hpp,v 1.1 2008/06/21 22:25:10 kpharris Exp $
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

#if       !defined(AMETHYST__STRING_DUMPABLE_HPP)
#define            AMETHYST__STRING_DUMPABLE_HPP

#include <string>
#include <iosfwd>

namespace amethyst
{

  /**
   *
   * The base class for string dumpable objects.  Anything derived
   * from this can have its members converted (one way) to a string.
   *
   * @author Kevin Harris <kpharris@users.sourceforge.net>
   * @version $Revision: 1.1 $
   *
   */
  class string_dumpable
  {
  public:
    string_dumpable();
    virtual ~string_dumpable();

    virtual std::string internal_members(const std::string& indentation, bool prefix_with_classname = false) const = 0;

    virtual std::string to_string(const std::string& base_indentation = "",
		const std::string& level_indentation = "  ") const;

    virtual std::string name() const = 0;

  }; // class string_dumpable

   std::ostream& operator<<(std::ostream& o, const string_dumpable& s);
   std::string inspect(const string_dumpable& s);
} // namespace amethyst


#endif /* !defined(AMETHYST__STRING_DUMPABLE_HPP) */

