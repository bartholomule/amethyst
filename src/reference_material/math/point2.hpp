/*
 * $Id: point2.hpp,v 1.2 2004/02/14 06:55:18 kpharris Exp $
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

#if    !defined(AMETHYST__POINT2_HPP)
#define         AMETHYST__POINT2_HPP

/*
  point2.hpp

  A 2d equiv of the stuff in point3.hpp
*/

/*
  revision history:
  17Jan1999 Wrote this file, did some basic testing.
  20jan1999 Added a default constructor.
  02feb1999 Fixed the broken constructor.
  18May2001 "Templatized" the class...
  17Sep2003 Changed the inheritence from public to private.  Added accessors,
            fixed implementation to use those accessors.    
*/

#include "coord2.hpp"
#include "vector2.hpp"

namespace amethyst
{ 

  template <class T>
  class point2: private coord2<T>
  {
  public:
    typedef T base;
    
    inline point2():coord2<T>() {}
    inline point2(T x, T y): coord2<T>(x,y){}
    inline explicit point2(const coord2<T>& c): coord2<T>(c) {}
    inline point2(const point2<T>& p): coord2<T>(p) {}    

    inline void set(T x, T y) { coord2<T>::set(x,y); }
    
    /* Accessors */
    inline T& operator[](int coord_index)
    {
      return(coord2<T>::operator[](coord_index));
    }
  
    inline T  operator[](int coord_index) const
    {
      return(coord2<T>::operator[](coord_index));
    }

    T& x()       { return(coord2<T>::x()); }
    T  x() const { return(coord2<T>::x()); }
    T& y()       { return(coord2<T>::y()); }
    T  y() const { return(coord2<T>::y()); }

    point2<T>& operator=(const point2<T>& p);
    point2<T>& operator+=(const vector2<T>& v1);
    point2<T>& operator-=(const vector2<T>& v1);

  }; // class point2

  template <class T>
  point2<T>& point2<T>::operator=(const point2<T>& p)
  {
    set( p.x(), p.y() );

    return *this;
  }
  
  template <class T>
  point2<T>& point2<T>::operator+=(const vector2<T>& v1)
  {
    set( x() + v1.x(),
	 y() + v1.y() );
    return *this;
  }

  template <class T>
  point2<T>& point2<T>::operator-=(const vector2<T>& v1)
  {
    set( x() - v1.x(),
	 y() - v1.y() );    
    return *this;
  }  

  template <class T>
  inline vector2<T> operator-(const point2<T>& p1, const point2<T>& p2)
  {
    return vector2<T>(p1.x() - p2.x(), p1.y() - p2.y());
  }
  
  template <class T>
  inline point2<T>  operator+(const point2<T>& p1, const vector2<T>& v1)
  {
    return point2<T>(p1.x() + v1.x(), p1.y() + v1.y());
  }

  template <class T>
  inline point2<T>  operator-(const point2<T>& p1, const vector2<T>& v1)
  {
    return point2<T>(p1.x() - v1.x(), p1.y() - v1.y());    
  }

  template <class T>
  std::ostream& operator << (std::ostream& o, const point2<T>& p)
  {
    o << "(" << p.x() << "," << p.y() << ")";
    return(o);
  }

  
} // namespace amethyst

#endif /* !defined(AMETHYST__POINT2_HPP) */
