#if    !defined(KH_VECTOR2_H)
#define         KH_VECTOR2_H

/*
  vector2.h

  The 2d equiv of the stuff in vector3.h
*/

/*
  Revision history:
  17Jan1999 Wrote this file, did some basic testing.
  20jan1999 Added a default constructor.
  18May2001 "Templatized" the class...
*/

#include "coord2.h"
#include "point2.h"

// FIXME! Should not be public derivation!

template <class T>
class vector2:public coord2<T>
{
public:
  inline vector2(): coord2<T>() {}
  inline vector2(T x, T y): coord2(x,y){}
  inline vector2(const coord2<T>& c): coord2<T>(c) {}

  friend ostream& operator << (ostream& o, const vector<T>2& v)
  {
    o << "<" << v.x() << "," << v.y() << ">";
    return(o);
  }
};

#endif /* !defined(KH_VECTOR2_H) */




