#if    !defined(KH_POINT2_H)
#define         KH_POINT2_H

/*
  coord2.h

  A 2d equiv of the stuff in point3.h
*/

/*
  revision history:
  17Jan1999 Wrote this file, did some basic testing.
  20jan1999 Added a default constructor.
  02feb1999 Fixed the broken constructor.
  18May2001 "Templatized" the class...
*/

// FIXME! Should not be public derivation!

#include "coord2.h"
#include "vector2.h"

template <class T>
class point2: public coord2<T>
{
public:
  inline point2():coord2<T>() {}
  inline point2(T x, T y): coord2<T>(x,y){}
  inline point2(const coord2<T>& c): coord2<T>(c) {}

  inline friend vector2<T> operator-(const point2<T>& p1, const point2<T>& p2)
  {
    vector2<T> c(p1); c -= p2; return(c);
  }
  inline friend point2<T>  operator+(const point2<T>& p1, const vector2<T>& v1)
  {
    point2<T> c(p1); c += v1; return(c);
  }
  inline friend point2<T>  operator-(const point2<T>& p1, const vector2<T>& v1)
  {
    point2<T> c(p1); c -= v1; return(c);
  }

  friend ostream& operator << (ostream& o, const point2<T>& p)
  {
    o << "(" << p.x() << "," << p.y() << ")";
    return(o);
  }
  
};


#endif /* !defined(KH_POINT2_H) */




