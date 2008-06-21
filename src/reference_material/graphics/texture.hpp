/*
 * $Id: texture.hpp,v 1.1 2008/06/21 22:25:10 kpharris Exp $
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

#if       !defined(AMETHYST__TEXTURE_HPP)
#define            AMETHYST__TEXTURE_HPP

#include "amethyst/math/point3.hpp"
#include "amethyst/math/coord2.hpp"
#include "amethyst/general/string_dumpable.hpp"

namespace amethyst
{

  /**
   *
   * The base class for a texture.  A texture in itself is not useful for
   * shading an object.  A material is used to assign textures for objects.
   *
   * @author Kevin Harris <kpharris@users.sourceforge.net>
   * @version $Revision: 1.1 $
   *
   */
  template<class T, class color_type>
  class texture : public string_dumpable
  {

  private:

  protected:

  public:
    /** Default constructor */
    texture();

    /** Destructor */
    virtual ~texture();

    virtual color_type get_color(const point3<T>& location, const coord2<T>& coord) const = 0;

    virtual std::string name() const
    {
      return "texture";
    }

  }; // class texture

  //--------------------------------------
  // Default constructor for class texture
  //--------------------------------------
  template<class T, class color_type>
  texture<T,color_type>::texture()
  {
  } // texture()

  //-----------------------------
  // Destructor for class texture
  //-----------------------------
  template<class T, class color_type>
  texture<T,color_type>::~texture()
  {
  } // ~texture()

} // namespace amethyst


#endif /* !defined(AMETHYST__TEXTURE_HPP) */

