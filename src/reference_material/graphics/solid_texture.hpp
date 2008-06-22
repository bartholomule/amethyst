/*
 * $Id: solid_texture.hpp,v 1.2 2008/06/22 17:27:07 kpharris Exp $
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

#if       !defined(AMETHYST__SOLID_TEXTURE_HPP)
#define            AMETHYST__SOLID_TEXTURE_HPP

#include "amethyst/graphics/texture.hpp"

namespace amethyst
{

  /**
   *
   * A class for a solid texture.  These textures generate colors based on 3d
   * positioning.
   *
   * @author Kevin Harris <kpharris@users.sourceforge.net>
   * @version $Revision: 1.2 $
   *
   */
  template<class T, class color_type>
  class solid_texture : public texture<T,color_type>
  {

  public:
    solid_texture();
    virtual ~solid_texture();

    color_type get_color(const point3<T>& location, const coord2<T>& coord) const;
    virtual color_type get_color_at_point(const point3<T>& location) const = 0;

    virtual std::string name() const
    {
      return "texture";
    }

  }; // class solid_texture

  template<class T, class color_type>
  solid_texture<T,color_type>::solid_texture():
   texture<T,color_type>()
  {
  } // solid_texture()

  template<class T, class color_type>
  solid_texture<T,color_type>::~solid_texture()
  {
  } // ~solid_texture()

  template<class T, class color_type>
  color_type solid_texture<T,color_type>::get_color(const point3<T>& location, const coord2<T>& coord) const
  {
    return get_color_at_point(location);
  }

} // namespace amethyst


#endif /* !defined(AMETHYST__SOLID_TEXTURE_HPP) */

