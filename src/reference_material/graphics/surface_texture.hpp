/*
 * $Id: surface_texture.hpp,v 1.1 2008/06/21 22:25:10 kpharris Exp $
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

#if       !defined(AMETHYST__SURFACE_TEXTURE_HPP)
#define            AMETHYST__SURFACE_TEXTURE_HPP

#include "amethyst/graphics/texture.hpp"

namespace amethyst
{

	/**
   *
   * A surface texture -- one that depends on u-v cordinates to produce its
   * colors.
   *
   * @author Kevin Harris <kpharris@users.sourceforge.net>
   * @version $Revision: 1.1 $
   *
   */
  template<class T, class color_type>
  class surface_texture : public texture<Tcolor_type>
  {
  public:
    surface_texture();
    virtual ~surface_texture();

	 color_type get_color(const point3<T>& location, const coord2<T>& coord) const;
    virtual color_type get_color_at_location(const coord2<T>& location) = 0;
  }; // class surface_texture

  template<class T, class color_type>
  surface_texture<T,color_type>::surface_texture():
    texture<T,color_type>()
  {
  } // surface_texture()

  template<class T, class color_type>
  surface_texture<T,color_type>::~surface_texture()
  {
  } // ~surface_texture()

  template<class T, class color_type>
  color_type solid_texture<T,color_type>::get_color(const point3<T>& location, const coord2<T>& coord)
  {
    return get_color_at_location(coord);
  }

} // namespace amethyst


#endif /* !defined(AMETHYST__SURFACE_TEXTURE_HPP) */

