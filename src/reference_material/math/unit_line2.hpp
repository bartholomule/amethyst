/*
 * $Id: unit_line2.hpp,v 1.4 2004/04/07 05:10:06 kpharris Exp $
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

#if       !defined(AMETHYST__UNIT_LINE2_HPP)
#define            AMETHYST__UNIT_LINE2_HPP

#include "line2.hpp"
#include <limits>

namespace amethyst
{ 

  /**
   * 
   * A simple 2d line with a unit direction.
   * 
   * @author Kevin Harris <kpharris@users.sourceforge.net>
   * @version $Revision: 1.4 $
   * 
   */
  template<class T>
  class unit_line2
  {

  private:

  protected:
    point2<T> line_origin;
    vector2<T> line_direction;
    interval<T> line_limits;
    bool infinite_limits;
    T non_unit_length;

  public:
    /** Default constructor */
    unit_line2();

    unit_line2(const point2<T>& o, const vector2<T>& v,
               const interval<T>& limits = interval<T>( numeric_limits<T>::min(),
                                                        numeric_limits<T>::max() ));
    unit_line2(const point2<T>& p1, const point2<T>& p2);

    unit_line2(const line2<T>& line);

    /** Destructor */
    virtual ~unit_line2();

    /** Copy constructor */
    unit_line2(const unit_line2& old);

    /** Assignment operator */
    unit_line2& operator= (const unit_line2& old);

    point2<T> o() const
    {
      return line_origin;
    }
    point2<T> origin() const
    {
      return line_origin;
    }

    vector2<T> v() const
    {
      return line_direction;
    }
    vector2<T> direction() const
    {
      return line_direction;
    }

    point2<T> minimum() const
    {
      return(line_origin +
             line_limits.begin() * direction());
    }
    point2<T> maximum() const
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

    operator line2<T>() const;

    T normal_length() const
    {
      return non_unit_length;
    }

    bool inside(T t) const
    {
      return line_limits.inside(t);
    }

  }; // class unit_line2



  //-----------------------------------
  // Default constructor for class unit_line2
  //-----------------------------------
  template<class T>
  unit_line2<T>::unit_line2():
    line_origin(),
    line_direction(),
    line_limits( -numeric_limits<T>::max(),
                 numeric_limits<T>::max ),
    infinite_limits(true)
  {

  } // unit_line2()

  template<class T>
  unit_line2<T>::unit_line2(const point2<T>& o, const vector2<T>& v,
                            const interval<T>& limits):
    line_origin(o),
    line_direction(v),
    line_limits(limits)
  {
    // Yes, I know that floating-point comparisons of this sort is usually bad,
    // but if they set it to something other than the default, this should
    // fail, so this wouldn't be infinite.
    infinite_limits = ( (line_limits.begin() <= -numeric_limits<T>::max()) &&
                        (line_limits.end() >= numeric_limits<T>::max()) );

    non_unit_length = line_direction.length();
    line_direction /= non_unit_length;

    if( !infinite_limits )
    {
      line_limits.set( line_limits.begin() * d,
                       line_limits.end() * d );
    }
  } // unit_line2()

  template<class T>
  unit_line2<T>::unit_line2(const point2<T>& p1, const point2<T>& p2):
    line_origin(p1),
    line_direction(p2 - p1),
    line_limits( numeric_limits<T>::min(),
                 T(1) - numeric_limits<T>::epsilon() ),
    infinite_limits(false)
  {
    non_unit_length = line_direction.length();
    line_direction /= non_unit_length;
    line_limits.set( line_limits.begin() * d,
                     line_limits.end() * d );
  } // unit_line2()  


  template <class T>
  unit_line2<T>::unit_line2(const line2<T>& line):
    line_origin(line.origin()),
    line_direction(line.direction()),
    line_limits( line.limits() )
  {
    infinite_limits = line.infinite();

    non_unit_length = line_direction.length();
    line_direction /= non_unit_length;

    if( !infinite_limits )
    {
      line_limits.set( line_limits.begin() * non_unit_length,
                       line_limits.end() * non_unit_length );
    }
  }

  //--------------------------
  // Destructor for class unit_line2
  //--------------------------
  template<class T>
  unit_line2<T>::~unit_line2()
  {

  } // ~unit_line2()

  //--------------------------------
  // Copy constructor for class unit_line2
  //--------------------------------
  template<class T>
  unit_line2<T>::unit_line2(const unit_line2<T>& old):
    line_origin(old.line_origin),
    line_direction(old.line_direction),
    line_limits(old.line_limits),
    infinite_limits(old.infinite_limits)
  {

  } // unit_line2(unit_line2)


  //-----------------------------------
  // Assignment operator for class unit_line2
  //-----------------------------------
  template<class T>
  unit_line2<T>& unit_line2<T>::operator= (const unit_line2<T>& old)
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
  } // unit_line2::operator=(unit_line2)

  template <class T>
  unit_line2<T>::operator line2<T>() const
  {
    if( infinite_limits )
    {
      return line2<T>(line_origin, line_direction * non_unit_length);
    }
    return line2<T>(line_origin,
                    line_direction * non_unit_length,
                    interval<T>( line_limits.begin() / non_unit_length,
                                 line_limits.end() / non_unit_length ));
  }

  template <class T>
  std::ostream& operator<<(std::ostream& o, const unit_line2<T>& l)
  {
    o << "|" << l.o() << ", " << l.v() << ", " << l.normal_length() << ", " << l.limits() << "|";
    return o;
  }

} // namespace amethyst


#endif /* !defined(AMETHYST__UNIT_LINE2_HPP) */

