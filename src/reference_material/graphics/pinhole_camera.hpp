/*
 * $Id: pinhole_camera.hpp,v 1.1 2004/04/18 21:21:06 kpharris Exp $
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

#if       !defined(AMETHYST__PINHOLE_CAMERA_HPP)
#define            AMETHYST__PINHOLE_CAMERA_HPP

// --------------------------------------
// Default include of parent class header
// --------------------------------------
#include "base_camera.hpp"

#include <math/frame.hpp>

namespace amethyst
{ 

  /**
   * 
   * A pinhole camera.
   * 
   * @author Kevin Harris <kpharris@users.sourceforge.net>
   * @version $Revision: 1.1 $
   * 
   */
  template<class T>
  class pinhole_camera : public base_camera<T>
  {

  private:
    frame<T> viewing_frame;
    coord2<T> ll_corner;
    coord2<T> ur_corner;
    T viewing_distance;
    coord2<T> vscreen_size;

  protected:

  public:
    /** Default constructor */
    pinhole_camera();

    /** Fully-specified constructor */
    pinhole_camera(const point3<T>& eye,
                   const vector3<T>& gaze,
                   const vector3<T>& up,
                   T virtual_screen_width,
                   T virtual_screen_height,
                   T virtual_screen_distance,
                   size_t w,
                   size_t h);

    /** Destructor */
    virtual ~pinhole_camera();

    /** Copy constructor */
    pinhole_camera(const pinhole_camera& old);

    /** Assignment operator */
    pinhole_camera& operator= (const pinhole_camera& old);

    /* FIXME! At some point in the future, these will need to have a more
       complete ray, which includes time, etc. */
    /* This one uses sample positions */
    virtual unit_line3<T> get_ray(const coord2<T>& sample_point) const;

    /* This one uses pixel positions */
    virtual unit_line3<T> get_ray(const T& px, const T& py) const;

    virtual std::string internal_members(const std::string& indentation, bool prefix_with_classname = false) const;
    
    virtual std::string to_string(const std::string& base_indentation,
                                  const std::string& level_indentation = "  ") const;

    virtual std::string to_string() const
    {
      return to_string("");
    }

    virtual std::string name() const
    {
      return "pinhole_camera";
    }
  }; // class pinhole_camera



  //---------------------------------------------
  // Default constructor for class pinhole_camera
  //---------------------------------------------
  template<class T>
  pinhole_camera<T>::pinhole_camera():
    base_camera<T>(100,100),
    viewing_frame(point3<T>(0,0,0),
                  vector3<T>(1,0,0), vector3<T>(0,1,0), vector3<T>(0,0,1)),
    ll_corner(-0.5,-0.5),
    ur_corner( 0.5, 0.5),
    viewing_distance(1),
    vscreen_size(ur_corner - ll_corner)
  {
  
  } // pinhole_camera()

  //-------------------------------------------------------
  // fully-specified  constructor for class pinhole_camera
  //-------------------------------------------------------
  template<class T>
  pinhole_camera<T>::pinhole_camera(const point3<T>& eye,
                                    const vector3<T>& gaze,
                                    const vector3<T>& up,
                                    T virtual_screen_width,
                                    T virtual_screen_height,
                                    T virtual_screen_distance,
                                    size_t w,
                                    size_t h):
    base_camera<T>(w,h),
    viewing_frame(eye, gaze, up),
    ll_corner(-virtual_screen_width / 2.0, -virtual_screen_height / 2.0),
    ur_corner( virtual_screen_width / 2.0,  virtual_screen_height / 2.0),
    viewing_distance(virtual_screen_distance),
    vscreen_size(virtual_screen_width, virtual_screen_height)
  {
  } // pinhole_camera()
  
  //------------------------------------
  // Destructor for class pinhole_camera
  //------------------------------------
  template<class T>
  pinhole_camera<T>::~pinhole_camera()
  {
    
  } // ~pinhole_camera()
  
  //------------------------------------------
  // Copy constructor for class pinhole_camera
  //------------------------------------------
  template<class T>
  pinhole_camera<T>::pinhole_camera(const pinhole_camera<T>& old):
    base_camera<T>(old),
    viewing_frame(old.viewing_frame),
    ll_corner(old.ll_corner),
    ur_corner(old.ur_corner),
    viewing_distance(old.viewing_distance),
    vscreen_size(old.vscreen_size)
  {

  } // pinhole_camera(pinhole_camera)

  //---------------------------------------------
  // Assignment operator for class pinhole_camera
  //---------------------------------------------
  template<class T>
  pinhole_camera<T>& pinhole_camera<T>::operator= (const pinhole_camera<T>& old)
  {
    // Generic check for self-assignment
    if( &old != this)
    {
      viewing_frame = old.viewing_frame;
      ll_corner = old.ll_corner;
      ur_corner = old.ur_corner;
      viewing_distance = old.viewing_distance;
      vscreen_size = old.vscreen_size;      

      base_camera<T>::operator=(old);
    }
    return (*this);
  } // pinhole_camera::operator=(pinhole_camera)

  template<class T>  
  unit_line3<T> pinhole_camera<T>::get_ray(const coord2<T>& sample_point) const
  {
    // Note: this flips both x and y, so that it looks like y increases as you
    // move up the screen, and x increases towards the right.  This conversion
    // may be faster to do at output time, however, this simplifies things.
    point3<T> view_point( (ll_corner.x() + (T(1) - sample_point.x()) * vscreen_size.x()),
			  (ll_corner.y() + (T(1) - sample_point.y()) * vscreen_size.y()),
			  viewing_distance );
    return unit_line3<T>( viewing_frame.origin(),
                          (viewing_frame.inverse_transform(view_point) -
			   viewing_frame.origin()));
  }

  
  template<class T>  
  unit_line3<T> pinhole_camera<T>::get_ray(const T& px, const T& py) const
  {
    // Flip x, so that it appears as though x increases to the right.
    T sx = (base_camera<T>::width() - px) / T(base_camera<T>::width() - 1);
    // Flip y, so that it appears as though y increases upwards.
    T sy = (base_camera<T>::height() - py) / T(base_camera<T>::height() - 1);

    point3<T> view_point( (ll_corner.x() + vscreen_size.x() * sx),
			  (ll_corner.y() + vscreen_size.y() * sy),
			  viewing_distance);

    return unit_line3<T>( viewing_frame.origin(),
                          (viewing_frame.inverse_transform(view_point) -
			   viewing_frame.origin()));
  }

  template <class T>
  std::string pinhole_camera<T>::internal_members(const std::string& indentation, bool prefix_with_classname) const
  {
    std::string retval;
    
    std::string internal_tagging = indentation;

    if( prefix_with_classname )
    {
      internal_tagging += pinhole_camera<T>::name() + "::";
    }
    retval += internal_tagging + string_format("frame=%1\n", viewing_frame);
    retval += internal_tagging + string_format("dist=%1\n", viewing_distance);
    retval += internal_tagging + string_format("size=%1\n", vscreen_size);
    retval += internal_tagging + string_format("ll corner=%1\n", ll_corner);
    retval += internal_tagging + string_format("ur corner=%1\n", ur_corner);

    return retval;
  }

  template <class T>
  std::string pinhole_camera<T>::to_string(const std::string& indent,
                                           const std::string& level_indent) const
  {
    return ( indent + pinhole_camera<T>::name() + "\n" + 
             indent + "{\n" +
             base_camera<T>::internal_members(indent + level_indent, true) +
             pinhole_camera<T>::internal_members(indent + level_indent, false) +
             indent + "}" );         
  }    
  
} // namespace amethyst


#endif /* !defined(AMETHYST__PINHOLE_CAMERA_HPP) */

