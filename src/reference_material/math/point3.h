#if    !defined(KH_POINT3_H)
#define         KH_POINT3_H

/*
  point3.h

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

#include "coord3.h"
#include "vector3.h"

template <class T>
class point3: private coord3<T>
{
public:
  inline point3(): coord3<T>() {}
  inline point3(T x, T y, T z): coord3<T>(x,y,z){}
  inline explicit point3(const coord3<T>& c): coord3<T>(c) {}
  inline point3(const point3<T>& c): coord3<T>(c) {}  

  /* Accessors */
  inline T& operator[](int coord_index)
  {
    return(coord3<T>::operator[](coord_index));
  }
  
  inline T  operator[](int coord_index) const
  {
    return(coord3<T>::operator[](coord_index));
  }

  T& x()       { return(coord3<T>::x()); }
  T  x() const { return(coord3<T>::x()); }
  T& y()       { return(coord3<T>::y()); }
  T  y() const { return(coord3<T>::y()); }
  T& z()       { return(coord3<T>::z()); }
  T  z() const { return(coord3<T>::z()); }  
  
  friend std::ostream& operator << (std::ostream& o, const point3<T>& p)
  {
    o << "(" << p.x() << "," << p.y() << "," << p.z() << ")";
    return(o);
  }
};

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


#endif /* !defined(KH_POINT3_H) */




