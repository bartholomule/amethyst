/*
 * $Id: triangle.hpp,v 1.3 2004/03/21 23:55:38 kpharris Exp $
 *
 * Part of "Amethyst" a playground for graphics development
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

#if       !defined(AMETHYST__TRIANGLE_HPP)
#define            AMETHYST__TRIANGLE_HPP

// --------------------------------------
// Include of parent class header
// --------------------------------------
#include "plane.hpp"

namespace amethyst
{ 

  /**
   * 
   * A simple triangle class, which is based on the plane class.
   * 
   * @author Kevin Harris <kpharris@users.sourceforge.net>
   * @version $Revision: 1.3 $
   * 
   */
  template<class T>
  class triangle : public plane<T>
  {

  private:

  protected:

  public:
    /** Default constructor */
    triangle();

    triangle(const point3<T>& corner1 = point3<T>(0,0,0),
	     const point3<T>& corner2 = point3<T>(1,0,0),
	     const point3<T>& corner3 = point3<T>(0,1,0));

    /** Destructor */
    virtual ~triangle();

    /** Copy constructor */
    triangle(const triangle& old);

    /** Assignment operator */
    triangle& operator= (const triangle& old);

    /** Returns if the given point is inside the shape. */
    virtual bool inside(const point3<T>& p) const;

    /** Returns if the given sphere intersects the shape. */
    virtual bool intersects(const sphere<T>& s) const;

    /** Returns if the given plane intersects the shape. */
    virtual bool intersects(const plane<T>& p) const;
    
    /** Returns if the given line intersects the plane. */
    virtual bool intersects_line(const unit_line3<T>& line,
				 intersection_info<T>& intersection) const;
    
    virtual std::string to_string(const std::string& indent,
				  const std::string& level_indent = "  ") const;
    virtual std::string name() const { return "triangle"; }
    
  }; // class triangle



  //---------------------------------------
  // Default constructor for class triangle
  //---------------------------------------
  template<class T>
  triangle<T>::triangle():
    plane<T>()
  {
  
  } // triangle()

  template<class T>
  triangle<T>::triangle(const point3<T>& corner1,
			const point3<T>& corner2,
			const point3<T>& corner3):
    plane<T>(corner1, corner2, corner3)
  {
  } // triangle(point3,point3,point3)

  
  //------------------------------
  // Destructor for class triangle
  //------------------------------
  template<class T>
  triangle<T>::~triangle()
  {
  
  } // ~triangle()

  //------------------------------------
  // Copy constructor for class triangle
  //------------------------------------
  template<class T>
  triangle<T>::triangle(const triangle<T>& old):
    plane<T>(old)
  {

  } // triangle(triangle)

  //---------------------------------------
  // Assignment operator for class triangle
  //---------------------------------------
  template<class T>
  triangle<T>& triangle<T>::operator= (const triangle<T>& old)
  {
    // Generic check for self-assignment
    if( &old != this)
    {
      plane<T>::operator=(old);
    }
    return (*this);
  } // triangle::operator=(triangle)

  template<class T>
  bool triangle<T>::inside(const point3<T>& point) const
  {
    coord2<T> uv;
    if( plane<T>::extract_uv_for_point(point, uv) )
    {
      if( (uv.x() > 0 && uv.y() > 0) &&
	  (uv.x() + uv.y() < 1) )
      {
	return true;
      }
    }
    return false;
  }
  
  template<class T>
  bool triangle<T>::intersects(const sphere<T>& s) const
  {
    // If any of the 3 corners is inside the sphere, the triangle intersects.
    if( s.inside( plane<T>::get_origin() ) ||
	s.inside( plane<T>::get_origin() + plane<T>::get_u_vector() ) ||
	s.inside( plane<T>::get_origin() + plane<T>::get_v_vector() ) )
    {
      return true;
    }
    return false;
  }
  
  template<class T>
  bool triangle<T>::intersects(const plane<T>& p) const
  {
    intersection_info<T> unused;
    const point3<T>& pp = plane<T>::get_origin();
    point3<T> pu = plane<T>::get_origin() + plane<T>::get_u_vector();
    point3<T> pv = plane<T>::get_origin() + plane<T>::get_v_vector();
    
    if( p.intersects_line( line3<T>(pp, pu), unused ) ||
	p.intersects_line( line3<T>(pp, pv), unused ) ||
	p.intersects_line( line3<T>(pu, pv), unused ) )
    {
      return true;
    }
    
    return false;
  }
  
  template<class T>
  bool triangle<T>::intersects_line(const unit_line3<T>& line,
				    intersection_info<T>& intersection) const
  {
    intersection_info<T> temp_intersection;

    if( plane<T>::intersects_line(line, temp_intersection) )
    {
      point3<T> p = (temp_intersection.get_distance() * line.direction() + line.origin());
      coord2<T> uv;
      if( plane<T>::extract_uv_for_point(p, uv) )
      {
	if( (uv.x() > 0 && uv.y() > 0) &&
	    (uv.x() + uv.y() < 1) )
	{
	  intersection = temp_intersection;
	  return true;
	}
      }      
    }
    return false;
  }

  template <class T>
  std::string triangle<T>::to_string(const std::string& indent,
				     const std::string& level_indent) const
  {
    
    return ( indent + "triangle\n" +
	     indent + "{\n" +
	     indent + level_indent + string_format("point=%1\n", plane<T>::get_origin()) +
	     indent + level_indent + string_format("u=%1\n", plane<T>::get_u_vector()) + 	     
	     indent + level_indent + string_format("v=%1\n", plane<T>::get_v_vector()) +
	     indent + level_indent + string_format("normal=%1\n", plane<T>::get_normal()) +
	     indent + "}" );
    
  }
  
} // namespace amethyst


#endif /* !defined(AMETHYST__TRIANGLE_HPP) */

