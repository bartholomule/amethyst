/*
 * $Id: simple_texture.hpp,v 1.2 2008/06/22 17:25:53 kpharris Exp $
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

#if       !defined(AMETHYST__SIMPLE_TEXTURE_HPP)
#define            AMETHYST__SIMPLE_TEXTURE_HPP

#include "amethyst/graphics/texture.hpp"

namespace amethyst
{
  /**
   *
   * A simple texture (constant color).
   *
   * @author Kevin Harris <kpharris@users.sourceforge.net>
   * @version $Revision: 1.2 $
   *
   */
  template<class T, class color_type>
  class simple_texture : public texture<T,color_type>
  {
  public:
    simple_texture(const color_type& color);
    virtual ~simple_texture();

    virtual color_type get_color(const point3<T>& location, const coord2<T>& coord) const;

    virtual std::string internal_members(const std::string& indentation, bool prefix_with_classname = false) const;
    virtual std::string name() const { return "simple_texture"; }

  private:
    color_type m_color;
  }; // class simple_texture

  template<class T, class color_type>
  simple_texture<T,color_type>::simple_texture(const color_type& color)
    : texture<T,color_type>()
    , m_color(color)
  {
  } // simple_texture()

  template<class T, class color_type>
  simple_texture<T,color_type>::~simple_texture()
  {
  } // ~simple_texture()

  template<class T, class color_type>
  color_type simple_texture<T,color_type>::get_color(const point3<T>& location, const coord2<T>& coord) const
  {
    return m_color;
  } // get_color

  template <class T, class color_type>
  std::string simple_texture<T,color_type>::internal_members(const std::string& indentation, bool prefix_with_classname) const
  {
    std::string retval;
    std::string internal_tagging = indentation;

    if( prefix_with_classname )
    {
      internal_tagging += simple_texture<T,color_type>::name() + "::";
    }

    retval += internal_tagging + string_format("color=%1\n", m_color);

    return retval;
  }

} // namespace amethyst


#endif /* !defined(AMETHYST__SIMPLE_TEXTURE_HPP) */

