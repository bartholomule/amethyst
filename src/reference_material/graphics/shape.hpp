/*
 * $Id: shape.hpp,v 1.3 2004/03/20 06:27:06 kpharris Exp $
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

#if       !defined(AMETHYST__SHAPE_HPP)
#define            AMETHYST__SHAPE_HPP

#include <math/point3.hpp>
#include <math/line3.hpp>
#include <math/unit_line3.hpp>
#include "intersection_info.hpp"

#include <string>
#include <iosfwd>

namespace amethyst
{ 
  template <class T> class sphere;
  template <class T> class plane;
  template <class T> bool intersects(const sphere<T>& s, const plane<T>& p);

  /**
   * 
   * The base class for a shape.
   * 
   * @author Kevin Harris <kpharris@users.sourceforge.net>
   * @version $Revision: 1.3 $
   * 
   */
  template<class T>
  class shape
  {

  private:

  protected:

  public:
    /** Default constructor */
    shape();

    /** Destructor */
    virtual ~shape();

    /** Copy constructor */
    shape(const shape& old);

    /** Assignment operator */
    shape& operator= (const shape& old);

    /** Returns if the given point is inside the shape. */
    virtual bool inside(const point3<T>& p) const = 0;

    /** Returns if the given sphere intersects the shape. */
    virtual bool intersects(const sphere<T>& s) const = 0;

    /** Returns if the given plane intersects the shape. */
    virtual bool intersects(const plane<T>& p) const = 0;

    /** Returns if the given line intersects the shape.  For performance
	reasons, this may be overridden, as the default just converts the
	line to a unit line, calls the unit version, and adjusts the
	distance for the scaling. */
    virtual bool intersects_line(const line3<T>& line,
				 intersection_info<T>& intersection) const;
    
    /** Returns if the given line intersects the shape. */
    virtual bool intersects_line(const unit_line3<T>& line,
				 intersection_info<T>& intersection) const = 0;

    virtual std::string to_string(const std::string& base_indentation,
				  const std::string& level_indentation = "  ") const = 0;
    
    virtual std::string to_string() const { return to_string(""); }

  }; // class shape



  //------------------------------------
  // Default constructor for class shape
  //------------------------------------
  template<class T>
  shape<T>::shape()
  {
  
  } // shape()

  //---------------------------
  // Destructor for class shape
  //---------------------------
  template<class T>
  shape<T>::~shape()
  {
  
  } // ~shape()

  //---------------------------------
  // Copy constructor for class shape
  //---------------------------------
  template<class T>
  shape<T>::shape(const shape<T>& old)
  {

  } // shape(shape)

  //------------------------------------
  // Assignment operator for class shape
  //------------------------------------
  template<class T>
  shape<T>& shape<T>::operator= (const shape<T>& old)
  {
    // Generic check for self-assignment
    if( &old != this)
    {
      // Insert assignment here. DELETEME

    }
    return (*this);
  } // shape::operator=(shape)

  template <class T>
  bool shape<T>::intersects_line(const line3<T>& line,
				 intersection_info<T>& intersection) const
  {
    unit_line3<T> ul(line);

    if( intersects_line(ul, intersection) )
    {
      intersection.set_distance(intersection.get_distance() /
				ul.normal_length());
      return true;
    }
    return false;
  } // shape::intersects_line(line3<T>,T)


  template <class T>
  std::ostream& operator<<(std::ostream& o, const shape<T>& s)
  {
    o << s.to_string();
    return o;
  }  
  
} // namespace amethyst


#endif /* !defined(AMETHYST__SHAPE_HPP) */

