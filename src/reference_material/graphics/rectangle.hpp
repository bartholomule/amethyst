/*
 * $Id: rectangle.hpp,v 1.1 2004/03/27 19:29:35 kpharris Exp $
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

#if       !defined(AMETHYST__RECTANGLE_HPP)
#define            AMETHYST__RECTANGLE_HPP

// --------------------------------------
// Include of parent class header
// --------------------------------------
#include "plane.hpp"

namespace amethyst
{ 

  /**
   * 
   * A simple rectangle class, which is based on the plane class.
   * 
   * @author Kevin Harris <kpharris@users.sourceforge.net>
   * @version $Revision: 1.1 $
   * 
   */
  template<class T>
  class rectangle : public plane<T>
  {

  private:

  protected:

  public:
    /** Default constructor */
    rectangle();

    rectangle(const point3<T>& point = point3<T>(0,0,0),
	      const vector3<T>& u = vector3<T>(1,0,0),
	      const vector3<T>& v = vector3<T>(0,1,0));

    rectangle(const point3<T>& corner1 = point3<T>(0,0,0),
	      const point3<T>& corner2 = point3<T>(1,0,0),
	      const point3<T>& corner3 = point3<T>(0,1,0));	      

    /** Destructor */
    virtual ~rectangle();

    /** Copy constructor */
    rectangle(const rectangle& old);

    /** Assignment operator */
    rectangle& operator= (const rectangle& old);

    /** Returns if the given point is inside the shape. */
    virtual bool inside(const point3<T>& p) const;

    /** Returns if the given sphere intersects the shape. */
    virtual bool intersects(const sphere<T>& s) const;

    /** Returns if the given plane intersects the shape. */
    virtual bool intersects(const plane<T>& p) const;
    
    /** Returns if the given line intersects the plane. */
    virtual bool intersects_line(const unit_line3<T>& line,
				 intersection_info<T>& intersection) const;

    virtual std::string internal_members(const std::string& indentation, bool prefix_with_classname = false) const;
    
    virtual std::string to_string(const std::string& indent,
				  const std::string& level_indent = "  ") const;
    virtual std::string name() const { return "rectangle"; }
    
  }; // class rectangle



  //---------------------------------------
  // Default constructor for class rectangle
  //---------------------------------------
  template<class T>
  rectangle<T>::rectangle():
    plane<T>()
  {
  
  } // rectangle()

  template <class T>
  rectangle<T>::rectangle(const point3<T>& point,
			  const vector3<T>& u,
			  const vector3<T>& v):
    plane<T>(point, u, v)
  {
  } // rectangle(point3,vector3,vector3)

  template<class T>
  rectangle<T>::rectangle(const point3<T>& corner1,
			const point3<T>& corner2,
			const point3<T>& corner3):
    plane<T>(corner1, corner2, corner3)
  {
  } // rectangle(point3,point3,point3)

  
  //------------------------------
  // Destructor for class rectangle
  //------------------------------
  template<class T>
  rectangle<T>::~rectangle()
  {
  
  } // ~rectangle()

  //------------------------------------
  // Copy constructor for class rectangle
  //------------------------------------
  template<class T>
  rectangle<T>::rectangle(const rectangle<T>& old):
    plane<T>(old)
  {

  } // rectangle(rectangle)

  //---------------------------------------
  // Assignment operator for class rectangle
  //---------------------------------------
  template<class T>
  rectangle<T>& rectangle<T>::operator= (const rectangle<T>& old)
  {
    // Generic check for self-assignment
    if( &old != this)
    {
      plane<T>::operator=(old);
    }
    return (*this);
  } // rectangle::operator=(rectangle)

  template<class T>
  bool rectangle<T>::inside(const point3<T>& point) const
  {
    coord2<T> uv;
    if( plane<T>::extract_uv_for_point(point, uv) )
    {
      if( (uv.x() > 0 && uv.y() > 0) &&
	  (uv.x() < 1) &&
	  (uv.y() < 1) )
      {
	return true;
      }
    }
    return false;
  }
  
  template<class T>
  bool rectangle<T>::intersects(const sphere<T>& s) const
  {
    // If any of the 4 corners is inside the sphere, the rectangle intersects.
    if( s.inside( plane<T>::get_origin() ) ||
	s.inside( plane<T>::get_origin() + plane<T>::get_u_vector() ) ||
	s.inside( plane<T>::get_origin() + plane<T>::get_v_vector() ) ||
	s.inside( plane<T>::get_origin() + plane<T>::get_u_vector() + plane<T>::get_v_vector() ) )
    {
      return true;
    }
    return false;
  }
  
  template<class T>
  bool rectangle<T>::intersects(const plane<T>& p) const
  {
    // This should be replaced with a much faster method.
    //  One potential method would be to project onto the plane to calculate
    //  the 'Z' coord, and check for differing signs on the 4 'Z' coordinates. 
    intersection_info<T> unused;
    const point3<T>& p1 = plane<T>::get_origin();
    point3<T> p2 = p1 + plane<T>::get_u_vector();
    point3<T> p3 = p2 + plane<T>::get_v_vector();
    point3<T> p4 = p1 + plane<T>::get_v_vector();
    
    if( p.intersects_line( line3<T>(p1, p2), unused ) ||
	p.intersects_line( line3<T>(p1, p4), unused ) ||
	p.intersects_line( line3<T>(p2, p3), unused ) ||
	p.intersects_line( line3<T>(p4, p3), unused ) ||
	// Just to be safe, check the diagonals.  This handles the case where
	// the rectangle is rotated 45 degrees so that the plane crosses
	// through two corner points. 
	p.intersects_line( line3<T>(p2, p4), unused ) ||
	p.intersects_line( line3<T>(p1, p3), unused ) )
    {
      return true;
    }
    
    return false;
  }
  
  template<class T>
  bool rectangle<T>::intersects_line(const unit_line3<T>& line,
				    intersection_info<T>& intersection) const
  {
    intersection_info<T> temp_intersection;

    if( plane<T>::intersects_line(line, temp_intersection) )
    {
      point3<T> p = ( temp_intersection.get_distance() * line.direction() + 
		      line.origin() );
      coord2<T> uv;
      if( plane<T>::extract_uv_for_point(p, uv) )
      {
	if( ( uv.x() > 0 ) &&
	    ( uv.y() > 0 ) &&
	    ( uv.x() < 1 ) &&
	    ( uv.y() < 1 ) )
	{
	  intersection = temp_intersection;
	  return true;
	}
      }      
    }
    return false;
  }

  template <class T>
  std::string rectangle<T>::internal_members(const std::string& indentation, bool prefix_with_classname) const
  {
    std::string retval;
    std::string internal_tagging = indentation;

    if( prefix_with_classname )
    {
      internal_tagging += rectangle<T>::name() + "::";
    }

    return retval;
  }

  template <class T>
  std::string rectangle<T>::to_string(const std::string& indent,
				     const std::string& level_indent) const
  {
    
    return ( indent + "rectangle\n" +
	     indent + "{\n" +
	     plane<T>::internal_members(indent + level_indent, true) +
	     rectangle<T>::internal_members(indent + level_indent, false) +
	     indent + "}" );
    
  }
  
} // namespace amethyst


#endif /* !defined(AMETHYST__RECTANGLE_HPP) */

