/*
 * $Id: point3.hpp,v 1.4 2007/05/19 07:55:52 kpharris Exp $
 *
 * Part of "Amethyst" a playground for graphics development
 * Copyright (C) 2003 Kevin Harris
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

#if    !defined(AMETHYST__POINT3_HPP)
#define         AMETHYST__POINT3_HPP

/*
  point3.hpp

  An expansion of the coord3 class to allow the creation of distinct types.

  It allows for creation of a vector by subtracting two points, and creation of
  another point by addition or subtraction of a vector to a point.
*/


/*
  Revision history:
  17jan1999 Wrote this file, did some basic testing.  Added the extraction
  operator to this, and the other '3' classes (vector3, coord3).
  This will make different output when dumped on a stream.  It could
  come in handy when debugging something.
  20jan1999 Added a default constructor.
  18May2001 "Templatized" the class...
  17Sep2003 Changed the inheritence from public to private.  Added accessors,
  fixed implementation to use those accessors.  
*/

#include "coord3.hpp"
#include "vector3.hpp"

namespace amethyst
{ 

  template <class T>
  class point3: private coord3<T>
  {
  public:
    typedef T base;

    inline point3(): coord3<T>()
    {
    }
    inline point3(T x, T y, T z): coord3<T>(x,y,z)
    {
    }
    inline explicit point3(const coord3<T>& c): coord3<T>(c)
    {
    }
    inline point3(const point3<T>& c): coord3<T>(c)
    {
    }

	  coord3<T> getcoord() const
	  {
		  return coord3<T>(x(), y(), z());
	  }

    inline void set(T x, T y, T z)
    {
      coord3<T>::set(x,y,z);
    }

    /* Accessors */
    inline T& operator[](int coord_index)
    {
      return(coord3<T>::operator[](coord_index));
    }

    inline T  operator[](int coord_index) const
    {
      return(coord3<T>::operator[](coord_index));
    }

    T& x()
    {
      return(coord3<T>::x());
    }
    T  x() const
    {
      return(coord3<T>::x());
    }
    T& y()
    {
      return(coord3<T>::y());
    }
    T  y() const
    {
      return(coord3<T>::y());
    }
    T& z()
    {
      return(coord3<T>::z());
    }
    T  z() const
    {
      return(coord3<T>::z());
    }  

    point3<T>& operator=(const point3<T>& p);
    point3<T>& operator+=(const vector3<T>& v1);
    point3<T>& operator-=(const vector3<T>& v1);

  }; // class point3

  template <class T>
  inline point3<T>& point3<T>::operator=(const point3<T>& p)
  {
    set(p.x(), p.y(), p.z());
    return(*this);
  }

  template <class T>
  inline point3<T>& point3<T>::operator+=(const vector3<T>& v1)
  {
    set(x() + v1.x(),
        y() + v1.y(),
        z() + v1.z());
    return(*this);
  }

  template <class T>
  inline point3<T>& point3<T>::operator-=(const vector3<T>& v1)
  {
    set(x() - v1.x(),
        y() - v1.y(),
        z() - v1.z());
    return(*this);
  }  


  template <class T>
  inline vector3<T> operator-(const point3<T>& p1, const point3<T>& p2)
  {
    return vector3<T>(p1.x() - p2.x(),
                      p1.y() - p2.y(),
                      p1.z() - p2.z());
  }
  template <class T>
  inline point3<T>  operator+(const point3<T>& p1, const vector3<T>& v1)
  {
    return point3<T>(p1.x() + v1.x(),
                     p1.y() + v1.y(),
                     p1.z() + v1.z());
  }
  template <class T>
  inline point3<T>  operator-(const point3<T>& p1, const vector3<T>& v1)
  {
    return point3<T>(p1.x() - v1.x(),
                     p1.y() - v1.y(),
                     p1.z() - v1.z());
  }
  template <class T>
  inline point3<T>  operator+(const vector3<T>& v1, const point3<T>& p1)
  {
    return point3<T>(p1.x() + v1.x(),
                     p1.y() + v1.y(),
                     p1.z() + v1.z());
  }
  template <class T>
  inline point3<T>  operator-(const vector3<T>& v1, const point3<T>& p1)
  {
    return point3<T>(v1.x() - p1.x(),
                     v1.y() - p1.y(),
                     v1.z() - p1.z());
  }  

  template <class T>
  std::ostream& operator << (std::ostream& o, const point3<T>& p)
  {
    o << "(" << p.x() << "," << p.y() << "," << p.z() << ")";
    return(o);
  }

} // namespace amethyst

#endif /* !defined(AMETHYST__POINT3_HPP) */
