/*
 * $Id: plane.hpp,v 1.1 2004/02/09 00:39:20 kpharris Exp $
 *
 * Part of "Many Games" - A nearly infinitely expandable gaming framework
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

#if       !defined(__AMETHYST__PLANE_HPP__)
#define            __AMETHYST__PLANE_HPP__

// --------------------------------------
// Default include of parent class header
// --------------------------------------
#include "shape.hpp"
#include "sphere.hpp"
#include <general/string_format.hpp>

namespace amethyst
{ 

  /**
   * 
   * A plane in 3d.
   * 
   * @author Kevin Harris <kpharris@users.sourceforge.net>
   * @version $Revision: 1.1 $
   * 
   */
  template<class T>
  class plane : public shape<T>
  {

  private:

    point3<T> defining_point;
    vector3<T> normal;
    
  protected:
    
  public:
    /** Default constructor */
    plane();

    /** Secondary (pos/orientation) constructor */
    plane(const point3<T>& p, const vector3<T>& n);
    
    /** Destructor */
    virtual ~plane();

    /** Copy constructor */
    plane(const plane& old);

    /** Assignment operator */
    plane& operator= (const plane& old);

    /** Returns if the given point is inside the shape. */
    virtual bool inside(const point3<T>& p) const;

    /** Returns if the given sphere intersects the shape. */
    virtual bool intersects(const sphere<T>& s) const;

    /** Returns if the given plane intersects the shape. */
    virtual bool intersects(const plane<T>& p) const;

    /** Returns if the given line intersects the sphere. */
    virtual bool intersects_line(const line3<T>& line, intersection_info<T>& intersection) const;
    
    /** Returns if the given line intersects the plane. */
    virtual bool intersects_line(const unit_line3<T>& line,
				 intersection_info<T>& intersection) const;

    virtual std::string to_string(const std::string& base_indentation,
				  const std::string& level_indentation = "  ") const;
    
  }; // class plane



  //------------------------------------
  // Default constructor for class plane
  //------------------------------------
  template<class T>
  plane<T>::plane():
    shape<T>(),
    defining_point(0,0,0),
    normal(0,0,0)
  {
  
  } // plane()

  //---------------------------------------------------------
  // Secondary (pos/orientation) constructor for class plane.
  //---------------------------------------------------------
  template <class T>
  plane<T>::plane(const point3<T>& p, const vector3<T>& n):
    shape<T>(),
    defining_point(p),
    normal(unit(n))
  {
    
  } // plane(point3,vector3)

  //---------------------------
  // Destructor for class plane
  //---------------------------
  template<class T>
  plane<T>::~plane()
  {
  
  } // ~plane()

  //---------------------------------
  // Copy constructor for class plane
  //---------------------------------
  template<class T>
  plane<T>::plane(const plane<T>& old):
    shape<T>(old),
    defining_point(old.defining_point),
    normal(old.normal)
  {

  } // plane(plane)

  //------------------------------------
  // Assignment operator for class plane
  //------------------------------------
  template<class T>
  plane<T>& plane<T>::operator= (const plane<T>& old)
  {
    // Generic check for self-assignment
    if( &old != this)
    {
      defining_point = old.defining_point;
      normal = old.normal;

      shape<T>::operator=(old);
    }
    return (*this);
  } // plane::operator=(plane)
  
  // Returns if the given point is inside the shape.
  template <class T>
  bool plane<T>:: inside(const point3<T>& p) const
  {
    T dist = dotprod(p - defining_point, normal);
    return (dist < AMETHYST_EPSILON) && (dist > -AMETHYST_EPSILON);
  }
  
  // Returns if the given sphere intersects the shape.
  template <class T>
  bool plane<T>:: intersects(const sphere<T>& s) const
  {
    T dist = dotprod(s.get_center() - defining_point, normal);
    T max_dist = s.get_radius() + AMETHYST_EPSILON;
    return (dist < max_dist) && (dist > -max_dist);
  }
  
  // Returns if the given plane intersects the shape.
  template <class T>
  bool plane<T>:: intersects(const plane<T>& p) const
  {
    T normal_proj = dotprod(normal, p.normal);

    // Flip it if required.
    if( normal_proj < T(0) )
    {
      normal_proj = -normal_proj;
    }

    // If they are not parallel, then they intersect somewhere.
    if( normal_proj < (1 - AMETHYST_EPSILON) )
    {
      return true;
    }
    // If they are parallel, but have the same defining point, then they
    // overlap EVERYWHERE (the same plane).
    else if( squared_length(defining_point - p.defining_point) <
	     AMETHYST_EPSILON )
    {
      return true;
    }
    // Anything else 
    else
    {
      return false;
    }
  }


  // Returns if the given line intersects the plane.
  template <class T>
  bool plane<T>::intersects_line(const unit_line3<T>& line,
				 intersection_info<T>& intersection) const
  {
    T ctheta = dotprod(line.direction(), normal);
    T t;

    // I will need to explain why this is safe to do.  If the angle is > 0 or
    // the angle is < 0, then we know there is a hit somewhere, and it can be
    // found.  If the angle is zero, then it will cause a divide by zero,
    // setting t to be +/-INF, which when compared to anything (even itself)
    // returns false, thus, the inside test below can be called with the INF,
    // and not get bad numbers out.
    //
    // DON'T DELETE THESE COMMENTS, AS THEY APPLY TO OTHER PLANE-BASED
    // FUNCTIONS AS WELL!
    if( ctheta > 0 )
    {
      t = (dotprod(defining_point - line.origin(), normal) /
	   ctheta);
    }
    else
    {
      t = (dotprod(defining_point - line.origin(), -normal) /
	   -ctheta);
    }

    if(line.inside(t))
    {
      intersection.set(this, t);
      return true;
    }
    
    return false;
  }

  // Returns if the given line intersects the plane.
  template <class T>
  bool plane<T>::intersects_line(const line3<T>& line,
				 intersection_info<T>& intersection) const
  {
    T ctheta = ( dotprod(line.direction(), normal) /
		 length(line.direction()) );
    T t;

    // Check the unit line version of this function for proper comments on why
    // this works!
    if( ctheta > 0 )
    {
      t = (dotprod(defining_point - line.origin(), normal) /
	   ctheta);
    }
    else
    {
      t = (dotprod(defining_point - line.origin(), -normal) /
	   -ctheta);
    }

    if(line.inside(t))
    {
      intersection.set(this, t);
      return true;
    }
    
    return false;
  }

  template <class T>
  std::string plane<T>::to_string(const std::string& indent,
				  const std::string& level_indent) const
  {
    return ( indent + "plane\n" +
	     indent + "{\n" +
	     indent + level_indent + string_format("point=%1\n", defining_point) + 
	     indent + level_indent + string_format("normal=%1\n", normal) +
	     indent + "}" );
	     
  }

  
  template <class T>
  bool intersects(const sphere<T>& s, const plane<T>& p)
  {
    return p.intersects(s);
  }
  
} // namespace amethyst


#endif /* !defined(__AMETHYST__PLANE_HPP__) */

