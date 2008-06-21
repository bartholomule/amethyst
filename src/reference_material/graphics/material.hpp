/*
 * $Id: material.hpp,v 1.1 2008/06/21 22:25:10 kpharris Exp $
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

#if       !defined(AMETHYST__MATERIAL_HPP)
#define            AMETHYST__MATERIAL_HPP

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
  template<typename color_type, typename T>
  class material : public string_dumpable
  {
  public:
    material();
    virtual ~material();

    virtual std::string internal_members(const std::string& indentation, bool prefix_with_classname = false) const;

    virtual std::string name() const
    {
      return "material";
    }

    virtual material_capabilities get_material_capabilties() const
    {
      return material_capabilities::NONE;
    }

	 // FIXME! Add functions for getting color, transforming for reflection (specular), transmission, etc.

  }; // class material

  template<typename color_type, typename T>
  material<color_type,T>::material()
  {
  } // material()

  template<typename color_type, typename T>
  material<color_type,T>::~material()
  {
  } // ~material()

	template <typename color_type, typename T>
	std::string material<color_type,T>::internal_members(const std::string& indentation, bool prefix_with_classname) const
	{
		std::string retval;
		std::string internal_tagging = indentation;

		if( prefix_with_classname )
		{
			internal_tagging += material<color_type,T>::name() + "::";
		}

		retval += indentation + string_format("capabilities=%1\n", get_material_capabilities().to_string());

		return retval;
	}

} // namespace amethyst


#endif /* !defined(AMETHYST__MATERIAL_HPP) */

