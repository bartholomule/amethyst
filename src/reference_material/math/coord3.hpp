/*
 * $Id: coord3.hpp,v 1.5 2004/04/18 21:36:49 kpharris Exp $
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

#if    !defined(AMETHYST__COORD3_HPP)
#define         AMETHYST__COORD3_HPP

/*
  coord3.hpp

  A 3d coordinate class that has some basic functionality.  Also, some
  functions to act on that class (at the bottom).
  
*/

/*
  Revision history:
  17Jan1999 Wrote this file, did some basic testing added the dot product,
  cross product, length, and unit functions.
  20jan1999 Added a default constructor for the coord3 class and the
  coord3_union union. 
  25jan1999 Added a unary - operator, and an invert function.
  05Mar1999 Added the * operator for two coord3s. It will take the
  componentwise product. 
  02Apr1999 Added the constructor to take a single double, it is intended to
  init the coord to a given value. 
  22Apr1999 Added the average function.
  Added the *= operator that takes two coord3s.
  18May2001 Started changing this for use in a newer tracer, as per "Realistic
  Raytracing" by Peter Shirley, "Templatized" the class...
  17Sep2003 Reordered the class, made it a part of namespace amethyst, changed
  extension to .hpp, added a GPL heading.
*/

#include <cmath>
#include <ostream>

namespace amethyst
{ 
  template <class T>
  class coord3
  {
  private:
    /* class that has 3 Ts stored directly */
    struct coord3_direct
    {
      T x; T y; T z;
    };
    /* class that has 3 Ts stored in an array */
    struct coord3_array
    {
      T coords[3];
    };
    /* union to allow accesses to both indirectly through an array, and directly
       through a name, without adding any extra processing time or space
       requirements */ 
    union  coord3_union
    {
      coord3_union()
      {
      }
      coord3_union(T x, T y, T z)
      {
        direct.x = x; direct.y = y; direct.z = z;
      }
      inline T& operator[](unsigned index)
      {
        return(array.coords[index]);
      }
      inline T  operator[](unsigned index) const
      {
        return(array.coords[index]);
      }
      coord3_direct direct;
      coord3_array  array;
    };

    /* The internal coordinates of this class */
    coord3_union coords;

  public:
    enum COORD_VALUES
      {
        X = 0, Y = 1, Z = 2
      };
    typedef T base;

    coord3()
    {
    }
    inline coord3(T u):coords(u,u,u)
    {
    }
    inline coord3(T x, T y, T z):coords(x,y,z)
    {
    }
    inline coord3(const coord3& old_coord):coords(old_coord.coords)
    {
    }

    inline void set(T x, T y, T z)
    {
      coords.direct.x = x;
      coords.direct.y = y;
      coords.direct.z = z;    
    }

    /* Accessors */
    inline T& operator[](unsigned coord_index)
    {
      return(coords[coord_index]);
    }

    inline T  operator[](unsigned coord_index) const
    {
      return(coords[coord_index]);
    }

    T& x()
    {
      return(coords.direct.x);
    }
    T  x() const
    {
      return(coords.direct.x);
    }
    T& y()
    {
      return(coords.direct.y);
    }
    T  y() const
    {
      return(coords.direct.y);
    }
    T& z()
    {
      return(coords.direct.z);
    }
    T  z() const
    {
      return(coords.direct.z);
    }

    coord3& operator= (const coord3& old_coord);
    coord3& operator+=(const coord3& p2);
    coord3& operator-=(const coord3& p2);
    coord3& operator*=(const coord3& p2); // Piecewise multiplication.
    coord3& operator*=(T factor);
    coord3& operator/=(T factor);
    template <class U>
    coord3& operator*=(U factor);
    template <class U>
    coord3& operator/=(U factor);        

    inline T length() const
    {
      return T(sqrt(double((x() * x()) +
                           (y() * y()) +
                           (z() * z()))));
    }  

  };

  template <class T>
  inline coord3<T>& coord3<T>::operator=(const coord3<T>& old_coord)
  {
    // No self-assignment comparison is done, because the compare/jump
    // should take longer than the assignment (for built-in types).
    coords.direct.x = old_coord.coords.direct.x;
    coords.direct.y = old_coord.coords.direct.y;
    coords.direct.z = old_coord.coords.direct.z;

    return *this;
  }


  template <class T>
  inline coord3<T>& coord3<T>::operator+=(const coord3<T>& p2)
  {
    coords.direct.x += p2.coords.direct.x;
    coords.direct.y += p2.coords.direct.y;
    coords.direct.z += p2.coords.direct.z;
    return *this;
  }

  template <class T>
  inline coord3<T>& coord3<T>::operator-=(const coord3<T>& p2)
  {
    coords.direct.x -= p2.coords.direct.x;
    coords.direct.y -= p2.coords.direct.y;
    coords.direct.z -= p2.coords.direct.z;
    return *this;
  }

  // Piecewise multiplication...
  template <class T>
  inline coord3<T>& coord3<T>::operator*=(const coord3<T>& p2)
  {
    coords.direct.x *= p2.coords.direct.x;
    coords.direct.y *= p2.coords.direct.y;
    coords.direct.z *= p2.coords.direct.z;
    return *this;    
  }

  template <class T>
  inline coord3<T>& coord3<T>::operator*=(T factor)
  {
    coords.direct.x *= factor;
    coords.direct.y *= factor;
    coords.direct.z *= factor;
    return *this;
  }

  template <class T>
  inline coord3<T>& coord3<T>::operator/=(T factor)
  {
    coords.direct.x /= factor;
    coords.direct.y /= factor;
    coords.direct.z /= factor;
    return *this;
  }

  template <class T>
  template <class U>
  inline coord3<T>& coord3<T>::operator*=(U factor)
  {
    coords.direct.x = T(coords.direct.x * factor);
    coords.direct.y = T(coords.direct.y * factor);
    coords.direct.z = T(coords.direct.z * factor);    
    return *this;
  }

  template <class T>
  template <class U>
  inline coord3<T>& coord3<T>::operator/=(U factor)
  {
    coords.direct.x = T(coords.direct.x / factor);
    coords.direct.y = T(coords.direct.y / factor);
    coords.direct.z = T(coords.direct.z / factor);    
    return *this;
  }  

  /* non-member math functions (also inlined for an attempt at some speed) */
  template <class T>
  inline coord3<T> operator+(const coord3<T>& p1, const coord3<T>& p2)
  {
    coord3<T> p(p1); p += p2; return(p);
  }

  template <class T>
  inline coord3<T> operator-(const coord3<T>& p1, const coord3<T>& p2)
  {
    coord3<T> p(p1); p -= p2; return(p);
  }

  template <class T>
  inline coord3<T> operator*(const coord3<T>& p1, T factor)
  {
    coord3<T> p(p1); p *= factor;  return(p);
  }

  template <class T>
  inline coord3<T> operator*(T factor, const coord3<T>& p1)
  {
    coord3<T> p(p1); p *= factor;  return(p);
  }

  template <class T, class U>
  inline coord3<T> operator*(const coord3<T>& p1, U factor)
  {
    coord3<T> p(p1); p *= factor;  return(p);
  }

  template <class T, class U>
  inline coord3<T> operator*(U factor, const coord3<T>& p1)
  {
    coord3<T> p(p1); p *= factor;  return(p);
  }

  template <class T>
  inline coord3<T> operator*(const coord3<T>& p1, const coord3<T>& p2)
  {
    return(coord3<T>(p1.x() * p2.x(),
                     p1.y() * p2.y(),
                     p1.z() * p2.z()));
  }
  template <class T>
  inline coord3<T> operator/(const coord3<T>& p1, T factor)
  {
    coord3<T> p(p1); p /= factor;  return(p);
  }

  template <class T, class U>
  inline coord3<T> operator/(const coord3<T>& p1, U factor)
  {
    coord3<T> p(p1); p /= factor;  return(p);
  }

  // Unary minus 
  template <class T>
  inline coord3<T> operator-(const coord3<T>& p1)
  {
    return(T(-1)*p1);
  }


  /* exports from this header file */
  template <class T>
  inline T dotprod(const coord3<T>& c1, const coord3<T>& c2)
  {
    return( (c1.x()*c2.x()) + (c1.y()*c2.y()) + (c1.z()*c2.z()));  
  }

  template <class T>
  inline coord3<T> crossprod(const coord3<T>& c1, const coord3<T>& c2)
  {
    return( coord3<T>((c1.y()*c2.z()) - (c2.y()*c1.z()),
                      (c2.x()*c1.z()) - (c1.x()*c2.z()),
                      (c1.x()*c2.y()) - (c2.x()*c1.y())));  
  }

  template <class T>
  inline T length(const coord3<T>& c)
  {
    return(c.length());
  }

  template <class T> inline T squared_length(const coord3<T>& c)
  {
    return(dotprod(c,c));
  }

  template <class T> inline coord3<T> unit(const coord3<T>& c)
  {
    return(c/c.length());
  }

  template <class T>
  std::ostream& operator << (std::ostream& o, const coord3<T>& c)
  {
    o << "{" << c.x() << "," << c.y() << "," << c.z() << "}";
    return(o);
  }

} // namespace amethyst

#endif /* !defined(AMETHYST__COORD3_HPP) */
