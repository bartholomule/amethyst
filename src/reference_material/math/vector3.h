#if    !defined(KH_VECTOR3_H)
#define         KH_VECTOR3_H

/*
  vector3.h

  This goes along with the point3 class, and has very few internal functions.
  It is intended for use as you would any vector. 
  
 */


/*
  Revision history:
  17jan1999 Wrote this file, did some basic testing.
  20jan1999 Added a default constructor.
  09Mar1999 Added the operator* to make the sgi compiler happy.  It was
            complaining that there were multiple ways to do the multiplication
	    operation by converting the vector to other types.
	    Added the - operator for the same reason.
  18May2001 "Templatized" the class...
  17Sep2003 Changed the inheritence from public to private.  Added accessors,
            fixed implementation to use those accessors.    
*/

template <class T>
class vector3;

#include "coord3.h"

template <class T>
class vector3:private coord3<T>
{
public:
  inline vector3():coord3<T>() {}
  inline vector3(T x, T y, T z): coord3<T>(x,y,z){}
  inline explicit vector3(const coord3<T>& c): coord3<T>(c) {}
  inline vector3(const vector3<T>& c): coord3<T>(c) {}  
  
  inline void set(T x, T y, T z)
  {
    cord3<T>::set(x,y,z);
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

  T& x()       { return(coord3<T>::x()); }
  T  x() const { return(coord3<T>::x()); }
  T& y()       { return(coord3<T>::y()); }
  T  y() const { return(coord3<T>::y()); }
  T& z()       { return(coord3<T>::z()); }
  T  z() const { return(coord3<T>::z()); }  
  

  inline vector3<T>& operator+=(const vector3<T>& p2)
  {
    x() += p2.x();
    y() += p2.y();
    z() += p2.z();
    return(*this);
  }
  
  inline vector3<T>& operator-=(const vector3<T>& p2)
  {
    x() -= p2.x();
    y() -= p2.y();
    z() -= p2.z();
    return(*this);
  }

  inline vector3<T>& operator*=(T d)
  {
    x() *= d;
    y() *= d;
    z() *= d;
    return(*this);
  }

  inline vector3<T>& operator/=(T d)
  {
    x() /= d;
    y() /= d;
    z() /= d;
    return(*this);
  }
  
  friend std::ostream& operator << (std::ostream& o, const vector3<T>& v)
  {
    o << "<" << v.x() << "," << v.y() << "," << v.z() << ">";
    return(o);
  }
};

template <class T>
inline coord3<T> operator-(const coord3<T>& p1)
{
  return(T(-1)*p1);
}

template <class T>
vector3<T> operator*(const vector3<T>& v, T d)
{
  vector3<T> temp(v);
  temp *= d;
  return temp;
  //  return vector3<T>(v.x() * d, v.y() * d, v.z() * d);
}

template <class T>
vector3<T> operator*(T d, const vector3<T>& v)
{
  vector3<T> temp(v);
  temp *= d;
  return temp;
  //  return vector3<T>(v.x() * d, v.y() * d, v.z() * d);
}

template <class T>
vector3<T> operator-(const vector3<T>& v1, const vector3<T>& v2)
{
  vector3<T> temp(v1);
  temp -= v2;
  return temp;
  //  return vector3<T>(v1.x() - v2.x(), v1.y() - v2.y(), v1.z() - v2.z());
}

template <class T>
vector3<T> operator+(const vector3<T>& v1, const vector3<T>& v2)
{
  vector3<T> temp(v1);
  temp += v2;
  return temp;
  //  return vector3<T>(v1.x() + v2.x(), v1.y() + v2.y(), v1.z() + v2.z());
}

template <class T>
T dotprod(const vector3<T>& v1, const vector3<T>& v2)
{
  return((v1.x() * v2.x()) +
	 (v1.y() * v2.y()) +
	 (v1.z() * v2.z()));
}

template <class T>
vector<T> crossprod(const vector3<T>& v1, const vector3<T>& v2)
{
  return vector3<T>(
		    (v1.y()*v2.z()) - (v2.y()*v1.z()),
		    (v2.x()*v1.z()) - (v1.x()*v2.z()),
		    (v1.x()*v2.y()) - (v2.x()*v1.y())
                   );
}

template <class T>
T length(const vector<T>& v)
{
  T squared_length = dotprod(v,v);
  return T(sqrt(squared_length));
}

// FIXME! Find an exception to throw...
template <class T>
vector3<T> unit(const vector3<T>& v)
{
  T len = length(v);
  // FIXME! Check the squared length >= 0
  return v / len;
}


#endif /* !defined(KH_VECTOR3_H) */
