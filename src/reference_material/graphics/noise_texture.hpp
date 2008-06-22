/*
 * $Id: noise_texture.hpp,v 1.1 2008/06/22 17:27:07 kpharris Exp $
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

#if       !defined(AMETHYST__NOISE_TEXTURE_HPP)
#define            AMETHYST__NOISE_TEXTURE_HPP

#include "amethyst/general/rc_pointer.hpp"
#include "amethyst/graphics/noise.hpp"
#include "amethyst/graphics/solid_texture.hpp"
#include "amethyst/graphics/interpolated_value.hpp"

namespace amethyst
{
  /**
    *
    * A texture that is based on solid noise.
    *
    * @author Kevin Harris <kpharris@users.sourceforge.net>
    * @version $Revision: 1.1 $
    *
    */
  template<typename T, typename color_type>
  class noise_texture : public solid_texture<T,color_type>
  {
  public:
    typedef rc_pointer<interpolated_value<T, color_type> > interp_type;

    noise_texture(T scale = T(1), const rc_pointer<Random<T> >& rnd = rc_pointer<Random<T> >(new default_random<T>()));
    noise_texture(const color_type& c0, const color_type& c1, T scale = T(1), const rc_pointer<Random<T> >& rnd = rc_pointer<Random<T> >(new default_random<T>()));
    noise_texture(const interp_type& colors, T scale = T(1), const rc_pointer<Random<T> >& rnd = rc_pointer<Random<T> >(new default_random<T>()));
 
    virtual ~noise_texture();

    virtual color_type get_color_at_point(const point3<T>& location) const;

    virtual std::string internal_members(const std::string& indentation, bool prefix_with_classname = false) const;
    virtual std::string name() const { return "noise_texture"; }
  private:
    T m_scale;
    noise<T> m_noise;
    interp_type m_colors;

  }; // class noise_texture

  template<typename T, typename color_type>
  noise_texture<T,color_type>::noise_texture(T scale, const rc_pointer<Random<T> >& rnd)
    : solid_texture<T,color_type>()
    , m_scale(scale)
    , m_noise(rnd)
    , m_colors(create_interpolation<T,color_type>(color_type(0.8,0.0,0.0), color_type(0.0,0.0,0.8)).clone_new())
  {
  }

  template<typename T, typename color_type>
  noise_texture<T,color_type>::noise_texture(const color_type& c0, const color_type& c1, T scale, const rc_pointer<Random<T> >& rnd)
    : solid_texture<T,color_type>()
    , m_scale(scale)
    , m_noise(rnd)
    , m_colors(create_interpolation<T,color_type>(c0, c1).clone_new())
  {
  }

  template<typename T, typename color_type>
  noise_texture<T,color_type>::noise_texture(const typename noise_texture<T,color_type>::interp_type& colors, T scale, const rc_pointer<Random<T> >& rnd)
    : solid_texture<T,color_type>()
    , m_scale(scale)
    , m_noise(rnd)
    , m_colors(colors)
  {
  }

  template<typename T, typename color_type>
  noise_texture<T,color_type>::~noise_texture()
  {
  } // ~noise_texture()

  template <typename T, typename color_type>
  std::string noise_texture<T,color_type>::internal_members(const std::string& indentation, bool prefix_with_classname) const
  {
    std::string retval;
    std::string internal_tagging = indentation;

    if( prefix_with_classname )
    {
      internal_tagging += noise_texture<T,color_type>::name() + "::";
    }

    retval += internal_tagging + string_format("scale=%1\n", m_scale);

    if( m_colors )
    {
       retval += internal_tagging + string_format("colors=%1\n", m_colors->to_string(indentation, "  "));
    }
    else
    {
       retval += internal_tagging + "colors=NULL\n";
    }

    return retval;
  }

  template <typename T, typename color_type>
  color_type noise_texture<T,color_type>::get_color_at_point(const point3<T>& location) const
  {
     coord3<T> v(location.x() * m_scale, location.y() * m_scale, location.z() * m_scale);
     T noisy_value = (m_noise.value(v) + T(1)) / T(2);

     if( m_colors )
     {
        return m_colors->interpolate(noisy_value);
     }
     return color_type(0);
  }

} // namespace amethyst


#endif /* !defined(AMETHYST__NOISE_TEXTURE_HPP) */

