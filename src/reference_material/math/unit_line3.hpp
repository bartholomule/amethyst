/*
 * $Id: unit_line3.hpp,v 1.1 2004/02/08 23:59:42 kpharris Exp $
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

#if       !defined(__AMETHYST__UNIT_LINE3_HPP__)
#define            __AMETHYST__UNIT_LINE3_HPP__

#include "line3.hpp"
#include <limits>

namespace amethyst
{ 

  /**
   * 
   * A simple 3d line with a unit direction.
   * 
   * @author Kevin Harris <kpharris@users.sourceforge.net>
   * @version $Revision: 1.1 $
   * 
   */
  template<class T>
  class unit_line3
  {

  private:

  protected:
    point3<T> line_origin;
    vector3<T> line_direction;
    interval<T> line_limits;
    bool infinite_limits;
    T non_unit_length;
    
  public:
    /** Default constructor */
    unit_line3();

    unit_line3(const point3<T>& o, const vector3<T>& v,
	 const interval<T>& limits = interval<T>( -std::numeric_limits<T>::max(),
						  std::numeric_limits<T>::min() ));
    unit_line3(const point3<T>& p1, const point3<T>& p2);

    explicit unit_line3(const line3<T>& line);
    
    /** Destructor */
    virtual ~unit_line3();

    /** Copy constructor */
    unit_line3(const unit_line3& old);

    /** Assignment operator */
    unit_line3& operator= (const unit_line3& old);

    point3<T> o() const { return line_origin; }
    point3<T> origin() const { return line_origin; }
    
    vector3<T> v() const { return line_direction; }
    vector3<T> direction() const { return line_direction; }

    point3<T> minimum() const { return (line_origin +
					line_limits.begin() * direction()); }
    point3<T> maximum() const { return (line_origin +
					line_limits.end() * direction()); }    
    bool infinite() const { return infinite_limits; }

    interval<T> limits() const { return line_limits; }
    
    operator line3<T>() const;

    T normal_length() const { return non_unit_length; }

    bool inside(T t) const { return line_limits.inside(t); }

  }; // class unit_line3



  //-----------------------------------
  // Default constructor for class unit_line3
  //-----------------------------------
  template<class T>
  unit_line3<T>::unit_line3():
    line_origin(),
    line_direction(),
    line_limits( -std::numeric_limits<T>::max(),
		 std::numeric_limits<T>::max ),
    infinite_limits(true)
  {
  
  } // unit_line3()

  template<class T>
  unit_line3<T>::unit_line3(const point3<T>& o, const vector3<T>& v,
			    const interval<T>& limits):
    line_origin(o),
    line_direction(v),
    line_limits(limits)
  {
    // Yes, I know that floating-point comparisons of this sort is usually bad,
    // but if they set it to something other than the default, this should
    // fail, so this wouldn't be infinite.
    infinite_limits = ( (line_limits.begin() <= -std::numeric_limits<T>::max()) &&
			(line_limits.end() >= std::numeric_limits<T>::max()) );

    non_unit_length = line_direction.length();
    line_direction /= non_unit_length;

    if( !infinite_limits )
    {
      line_limits.set( line_limits.begin() * d,
		       line_limits.end() * d );
    }
  } // unit_line3()

  template<class T>
  unit_line3<T>::unit_line3(const point3<T>& p1, const point3<T>& p2):
    line_origin(p1),
    line_direction(p2 - p1),
    line_limits( std::numeric_limits<T>::min(),
		 T(1) - std::numeric_limits<T>::epsilon() ),
    infinite_limits(false)
  {
    non_unit_length = line_direction.length();
    line_direction /= non_unit_length;
    line_limits.set( line_limits.begin() * d,
		     line_limits.end() * d );
  } // unit_line3()  
  

  template <class T>
  unit_line3<T>::unit_line3(const line3<T>& line):
    line_origin(line.origin()),
    line_direction(line.direction()),
    line_limits( line.limits() )
  {
    infinite_limits = line.infinite();

    non_unit_length = length(line_direction);
    line_direction /= non_unit_length;

    if( !infinite_limits )
    {
      line_limits.set( line_limits.begin() * non_unit_length,
		       line_limits.end() * non_unit_length );
    }
  }
  
  //--------------------------
  // Destructor for class unit_line3
  //--------------------------
  template<class T>
  unit_line3<T>::~unit_line3()
  {
  
  } // ~unit_line3()

  //--------------------------------
  // Copy constructor for class unit_line3
  //--------------------------------
  template<class T>
  unit_line3<T>::unit_line3(const unit_line3<T>& old):
    line_origin(old.line_origin),
    line_direction(old.line_direction),
    line_limits(old.line_limits),
    infinite_limits(old.infinite_limits)
  {

  } // unit_line3(unit_line3)

  
  //-----------------------------------
  // Assignment operator for class unit_line3
  //-----------------------------------
  template<class T>
  unit_line3<T>& unit_line3<T>::operator= (const unit_line3<T>& old)
  {
    // Generic check for self-assignment
    if( &old != this)
    {
      line_origin = old.line_origin;
      line_direction = old.line_direction;
      line_limits = old.line_limits;
      infinite_limits = old.infinite_limits;
    }
    return (*this);
  } // unit_line3::operator=(unit_line3)

  template <class T>
  unit_line3<T>::operator line3<T>() const
  {
    if( infinite_limits )
    {
      return line3<T>(line_origin, line_direction * non_unit_length);
    }
    return line3<T>(line_origin,
		    line_direction * non_unit_length,
		    interval<T>( line_limits.begin() / non_unit_length,
				 line_limits.end() / non_unit_length ));
  }
  
} // namespace amethyst


#endif /* !defined(__AMETHYST__UNIT_LINE3_HPP__) */

