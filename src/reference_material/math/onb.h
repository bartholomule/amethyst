#if       !defined(KH_ONB_H)
#define            KH_ONB_H

/*
  onb.h -- A class to represent an ortho-normal basis
 */

/*
  Revision History:
  18May2001 Created this file.  Wrote constructors, accessors, and functions to
            convert to/from the ONB. 
 */

#include "coord3.h"

template <class T>
class onb
{
public:
  // An ONB where only the direction of w matters...
  inline onb(const coord3<T>& a):
    w(unit(a))
  {
    v = perp_vector(w);
    u = crossprod(v,w);
  }
  
  // An ONB where the direction of u,v, and w matter...
  // a and b define a plane, where a is in the direction of u.
  inline onb(const coord3<T>& a, const coord3<T>& b):
    v(unit(b)),
    w(unit(a))
  {
    u = crossprod(v,w);
  }

  inline onb(const onb<T>& o): u(o.u), v(o.v), w(o.w)
  {
  }
  
  inline onb<T>& operator=(const onb<T>& o)
  {
    if(&o != this)
    {
      u = o.u;
      v = o.v;
      w = o.w;
    }
    return(*this);
  }  

  // Take an external coord, and convert to one for this ONB
  inline coord3<T> coord_to_onb(const coord3<T>& c) const
  {
    return coord3<T>(dotprod(c,u),dotprod(c,v),dotprod(c,w));
  }
  // Take an internal coord, and convert to one for a 'global' ONB.
  inline coord3<T> coord_from_onb(const coord3<T>& c) const
  {
    return(u * c.x() + v * c.y() + w * c.z());
  }

  inline const coord3<T>& get_u() const { return u; }
  inline const coord3<T>& get_v() const { return v; }
  inline const coord3<T>& get_w() const { return w; }
  
  inline coord3<T>& get_u() { return u; }
  inline coord3<T>& get_v() { return v; }
  inline coord3<T>& get_w() { return w; }    
  
private:
  coord3<T> u, v, w;
};

#endif /* !defined(KH_ONB_H) */
