#if    !defined(KH_COORD2_H)
#define         KH_COORD2_H

/*
  coord2.h

  The 2d equiv of everything that I placed initially in coord3.h, except for
  the cross product, as there is no 2d equiv.
*/

/*
  Revision history:
  17Jan1999 Wrote this file, did some basic testing.
  20jan1999 Added a default constructor for the coord2 class.
            Added a default constructor for the union in the coord2 class.
  25jan1999 Added a unary - operator, and an invert function.
  18May2001 "Templatized" the class...
*/

#include <cmath>
#include <iosfwd>

template <class T>
class coord2
{
public:
  enum COORD_VALUES {X = 0, Y = 1};
  coord2() {}
  inline coord2(T x, T y):coords(x,y) {}
  inline coord2(const coord2<T>& old_coord):coords(old_coord.coords) {}

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

  /* math functions (inlined to attempt for some good speed) */
  inline coord2<T>& operator+=(const coord2<T>& p2)
  {
    coords.direct.x += p2.coords.direct.x;
    coords.direct.y += p2.coords.direct.y;
    return(*this);
  }
  
  inline coord2<T>& operator-=(const coord2<T>& p2)
  {
    coords.direct.x -= p2.coords.direct.x;
    coords.direct.y -= p2.coords.direct.y;
    return(*this);
  }

  inline coord2<T>& operator*=(T d)
  {
    coords.direct.x *= d;
    coords.direct.y *= d;
    return(*this);
  }

  inline coord2<T>& operator/=(T d)
  {
    coords.direct.x /= d;
    coords.direct.y /= d;
    return(*this);
  }



  /* Functions that more relate to vectors, but are needed */
  T dotprod(const coord2<T>& c) const
  {
    return( (x()*c.x()) + (y()*c.y()));
  }
  inline T length()          const
  {
    return(sqrt(dotprod(*this)));
  }  
  
  friend std::ostream& operator << <>(std::ostream& o, const coord2<T>& c);

private:
  /* class that has 2 Ts stored directly */
  struct coord2_direct { T x; T y; };
  /* class that has 2 Ts stored in an array */
  struct coord2_array { T coords[2]; };
  /* union to allow accesses to both indirectly through an array, and directly
     through a name, without adding any extra processing time or space
     requirements */ 
  union  coord2_union
  {
    coord2_union() {}
    coord2_union(T x, T y) { direct.x = x; direct.y = y; }
    T& operator[](int index)       { return(array.coords[index]); }
    T  operator[](int index) const { return(array.coords[index]); }
    coord2_direct direct;
    coord2_array  array;
  };

  /* The internal coordinates of this class */
  coord2_union coords;
};


/* friend math functions (also inlined for an attempt at some speed) */
template <class T>
inline coord2<T> operator+(const coord2<T>& p1, const coord2<T>& p2)
{
  coord2<T> p(p1); p += p2; return(p);
}
template <class T>
inline coord2<T> operator-(const coord2<T>& p1, const coord2<T>& p2)
{
  coord2<T> p(p1); p -= p2; return(p);
}
template <class T>
inline coord2<T> operator*(const coord2<T>& p1, T d)
{
  coord2<T> p(p1); p *= d;  return(p);
}
template <class T>
inline coord2<T> operator*(T d, const coord2<T>& p1)
{
  coord2<T> p(p1); p *= d;  return(p);
}
template <class T>
inline coord2<T> operator/(const coord2<T>& p1, T d)
{
  coord2<T> p(p1); p /= d;  return(p);
}
/* Unary friends */
template <class T>
inline coord2<T> operator-(const coord2<T>& p1)
{
  return(T(-1)*p1);
}


/* exports from this header file */
template <class T> inline T dotprod(const coord2<T>& c1, const coord2<T>& c2)
{
  return(c1.dotprod(c2));
}
template <class T> inline T length(const coord2<T>& c)
{
  return(c.length());
}
template <class T> inline coord2<T> unit(const coord2<T>& c)
{
  return(c/c.length());
}
template <class T> inline coord2<T> invert(const coord2<T>& c)
{
  return(coord2<T>(1/c.x(), 1/c.y()));
}
template <class T>
std::ostream& operator << (std::ostream& o, const coord2<T>& c)
{
  o << "{" << c.x() << "," << c.y() << "}";
  return(o);
}

#endif /* !defined(KH_COORD2_H) */




