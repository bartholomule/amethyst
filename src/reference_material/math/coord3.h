#if    !defined(KH_COORD3_H)
#define         KH_COORD3_H

/*
  coord3.h

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

*/

#include <cmath>
#include <iosfwd>
#include "template_functions.h"

template <class T>
class coord3
{
public:
  enum COORD_VALUES {X = 0, Y = 1, Z = 2};
  coord3() {}
  inline coord3(T u):coords(u,u,u) {}
  inline coord3(T x, T y, T z):coords(x,y,z) {}
  inline coord3(const coord3<T>& old_coord):coords(old_coord.coords) {}

  inline void set(T x, T y, T z)
  {
    coords.direct.x = x;
    coords.direct.y = y;
    coords.direct.z = z;    
  }
  
  /* Accessors */
  inline T& operator[](int coord_index)
  {
    return(coords[coord_index]);
  }
  
  inline T  operator[](int coord_index) const
  {
    return(coords[coord_index]);
  }

  T& x()       { return(coords.direct.x); }
  T  x() const { return(coords.direct.x); }
  T& y()       { return(coords.direct.y); }
  T  y() const { return(coords.direct.y); }
  T& z()       { return(coords.direct.z); }
  T  z() const { return(coords.direct.z); }

  /* math functions (inlined to attempt for some good speed) */
  inline coord3<T>& operator+=(const coord3<T>& p2)
  {
    coords.direct.x += p2.coords.direct.x;
    coords.direct.y += p2.coords.direct.y;
    coords.direct.z += p2.coords.direct.z;
    return(*this);
  }
  
  inline coord3<T>& operator-=(const coord3<T>& p2)
  {
    coords.direct.x -= p2.coords.direct.x;
    coords.direct.y -= p2.coords.direct.y;
    coords.direct.z -= p2.coords.direct.z;
    return(*this);
  }

  // Piecewise multiplication...
  inline coord3<T>& operator*=(const coord3<T>& p2)
  {
    coords.direct.x *= p2.coords.direct.x;
    coords.direct.y *= p2.coords.direct.y;
    coords.direct.z *= p2.coords.direct.z;
    return(*this);    
  }
  
  inline coord3<T>& operator*=(T d)
  {
    coords.direct.x *= d;
    coords.direct.y *= d;
    coords.direct.z *= d;
    return(*this);
  }

  inline coord3<T>& operator/=(T d)
  {
    coords.direct.x /= d;
    coords.direct.y /= d;
    coords.direct.z /= d;
    return(*this);
  }
  
  /* Functions that more relate to vectors, but are needed */
  T dotprod(const coord3<T>& c)   const
  {
    return( (x()*c.x()) + (y()*c.y()) + (z()*c.z()));
  }

  inline coord3<T> crossprod(const coord3<T>& c) const
  {
    return( coord3<T>(
		      (  y()*c.z()) - (c.y()*  z()),
		      (c.x()*  z()) - (  x()*c.z()),
		      (  x()*c.y()) - (c.x()*  y())
                     )
          );
  }

  inline T length()                   const
  {
    return(sqrt(dotprod(*this)));
  }  
  
private:
  /* class that has 3 Ts stored directly */
  struct coord3_direct { T x; T y; T z; };
  /* class that has 3 Ts stored in an array */
  struct coord3_array { T coords[3]; };
  /* union to allow accesses to both indirectly through an array, and directly
     through a name, without adding any extra processing time or space
     requirements */ 
  union  coord3_union
  {
    coord3_union() {}
    coord3_union(T x, T y, T z) { direct.x = x; direct.y = y; direct.z = z; }
    inline T& operator[](int index)       { return(array.coords[index]); }
    inline T  operator[](int index) const { return(array.coords[index]); }
    coord3_direct direct;
    coord3_array  array;
  };

  /* The internal coordinates of this class */
  coord3_union coords;
};



/* friend math functions (also inlined for an attempt at some speed) */
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
inline coord3<T> operator*(const coord3<T>& p1, T d)
{
  coord3<T> p(p1); p *= d;  return(p);
}
template <class T>
inline coord3<T> operator*(T d, const coord3<T>& p1)
{
  coord3<T> p(p1); p *= d;  return(p);
}
template <class T>
inline coord3<T> operator*(const coord3<T>& p1, const coord3<T>& p2)
{
  return(coord3<T>(p1.x() * p2.x(),
		   p1.y() * p2.y(),
		   p1.z() * p2.z()));
}
template <class T>
inline coord3<T> operator/(const coord3<T>& p1, T d)
{
  coord3<T> p(p1); p /= d;  return(p);
}
/* Unary friends */
template <class T>
inline coord3<T> operator-(const coord3<T>& p1)
{
  return(T(-1)*p1);
}


/* exports from this header file */
template <class T> inline T dotprod(const coord3<T>& c1, const coord3<T>& c2)
{
  return(c1.dotprod(c2));
}
template <class T> inline coord3<T> crossprod(const coord3<T>& c1, const coord3<T>& c2)
{
  return(c1.crossprod(c2));
}
template <class T> inline T length(const coord3<T>& c)
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

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// You can pretty much ignore anything below this, as it belongs elsewhere, and
// should be written in a more generic fashion. 
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
template <class T> inline coord3<T> invert(const coord3<T>& c)
{
  return(coord3<T>(1/c.x(), 1/c.y(), 1/c.z()));
}
template <class T> inline T average(const coord3<T>& c)
{
  return((c.x() + c.y() + c.z())/3);
}
template <class T> inline T max(const coord3<T>& c)
{
  return my_max(c.x(), my_max(c.y(),c.z()));
}
template <class T>
coord3<T> abs_vector(const coord3<T>& c)
{
  return coord3<T>(my_abs(c.x()), my_abs(c.y()), my_abs(c.z()));
}

template <class T> inline int min_abs_index(const coord3<T>& c)
{
  int min_index = 0;
  T min_abs = my_abs(c[min_index]);
  for(int i = 1; i < 3; ++i)
  {
    if(my_abs(c[i]) < min_abs)
    {
      min_index = i;
      min_abs = my_abs(c[min_index]);
    }
  }
  return(min_index);
}
template <class T> inline coord3<T> perp_vector(const coord3<T>& c)
{
  coord3<T> temp = unit(c);
  int zero_index = min_abs_index(temp);
  if(zero_index == 0)
  {
    return unit(coord3<T>(0,c.z(),-c.y()));
  }
  else if( zero_index == 1 )
  {
    return(unit(coord3<T>(c.z(),0,-c.x())));
  }
  else
  {
    return(unit(coord3<T>(c.y(),-c.x(),0)));
  }
}

// THIS IS CRAP, BUT MAY BE USED FOR SOME FORM OF SORTING...
template <class T> inline bool operator>(const coord3<T>& c1, const coord3<T>& c2)
{
  return dotprod(c1,c1) > dotprod(c2,c2);
}
template <class T> inline bool operator<(const coord3<T>& c1, const coord3<T>& c2)
{
  return dotprod(c1,c1) < dotprod(c2,c2);
}


template <class T>
std::ostream& operator << (std::ostream& o, const coord3<T>& c)
{
  o << "{" << c.x() << "," << c.y() << "," << c.z() << "}";
  return(o);
}


// This one does not neccecarily belong here, but it could be useful in many
// different things, so it has to go somewhere...
// Finds the best planar projection (axii), given the normal of the plane.
template <class T>
void best_planar_projection(const coord3<T>& normal, int& axis1, int& axis2)
{
  if((my_abx(normal.z()) > my_abs(normal.y())) &&
     (my_abs(normal.z()) > my_abs(normal.x())))
  {
    // X-Y projection best (z-major normal)
    axis1 = 0; axis2 = 1;
  }
  else if(my_abs(normal.y()) > my_abs(normal.x()))
  {
    // X-Z projection is best (Y-Major normal)
    axis1 = 0; axis2 = 2;
  }
  else
  {
    // Only case left: X-Major normal (y-z projection)
    axis1 = 1; axis2 = 2;
  }
}

#endif /* !defined(KH_COORD3_H) */




