/*
 * $Id: line3.hpp,v 1.4 2004/04/07 05:10:06 kpharris Exp $
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

#if       !defined(AMETHYST__LINE3_HPP)
#define            AMETHYST__LINE3_HPP

#include "point3.hpp"
#include "vector3.hpp"
#include "interval.hpp"

#include <limits>

namespace amethyst
{ 

  /**
   * 
   * A simple 3d line.
   * 
   * @author Kevin Harris <kpharris@users.sourceforge.net>
   * @version $Revision: 1.4 $
   * 
   */
  template<class T>
  class line3
  {

  private:

  protected:
    point3<T> line_origin;
    vector3<T> line_direction;
    interval<T> line_limits;
    bool infinite_limits;

  public:
    /** Default constructor */
    line3();

    line3(const point3<T>& o, const vector3<T>& v,
          const interval<T>& limits = interval<T>( std::numeric_limits<T>::min(),
                                                   std::numeric_limits<T>::max() ));

    line3(const point3<T>& p1, const point3<T>& p2);

    /** Destructor */
    virtual ~line3();

    /** Copy constructor */
    line3(const line3& old);

    /** Assignment operator */
    line3& operator= (const line3& old);

    point3<T> o() const
    {
      return line_origin;
    }
    point3<T> origin() const
    {
      return line_origin;
    }

    vector3<T> v() const
    {
      return line_direction;
    }
    vector3<T> direction() const
    {
      return line_direction;
    }

    point3<T> minimum() const
    {
      return(line_origin +
             line_limits.begin() * direction());
    }
    point3<T> maximum() const
    {
      return(line_origin +
             line_limits.end() * direction());
    }    

    bool infinite() const
    {
      return infinite_limits;
    }

    interval<T> limits() const
    {
      return line_limits;
    }

    bool inside(T t) const
    {
      return line_limits.inside(t);
    }

  }; // class line3



  //-----------------------------------
  // Default constructor for class line3
  //-----------------------------------
  template<class T>
  line3<T>::line3():
    line_origin(),
    line_direction(),
    line_limits( -std::numeric_limits<T>::max(),
                 std::numeric_limits<T>::max ),
    infinite_limits(true)
  {

  } // line3()

  template<class T>
  line3<T>::line3(const point3<T>& o, const vector3<T>& v,
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

  } // line3()

  template<class T>
  line3<T>::line3(const point3<T>& p1, const point3<T>& p2):
    line_origin(p1),
    line_direction(p2 - p1),
    line_limits( std::numeric_limits<T>::min(),
                 T(1) - std::numeric_limits<T>::epsilon() ),
    infinite_limits(false)
  {

  } // line3()  


  //--------------------------
  // Destructor for class line3
  //--------------------------
  template<class T>
  line3<T>::~line3()
  {

  } // ~line3()

  //--------------------------------
  // Copy constructor for class line3
  //--------------------------------
  template<class T>
  line3<T>::line3(const line3<T>& old):
    line_origin(old.line_origin),
    line_direction(old.line_direction),
    line_limits(old.line_limits),
    infinite_limits(old.infinite_limits)
  {

  } // line3(line3)

  //-----------------------------------
  // Assignment operator for class line3
  //-----------------------------------
  template<class T>
  line3<T>& line3<T>::operator= (const line3<T>& old)
  {
    // Generic check for self-assignment
    if( &old != this )
    {
      line_origin = old.line_origin;
      line_direction = old.line_direction;
      line_limits = old.line_limits;
      infinite_limits = old.infinite_limits;
    }
    return(*this);
  } // line3::operator=(line3)

  template <class T>
  std::ostream& operator<<(std::ostream& o, const line3<T>& l)
  {
    o << "|" << l.o() << ", " << l.v() << "|";
    return o;
  }

} // namespace amethyst


#endif /* !defined(AMETHYST__LINE3_HPP) */

