/*
 * $Id: onb.hpp,v 1.2 2003/09/18 20:36:59 kpharris Exp $
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
#if       !defined(__AMETHYST__ONB_HPP__)
#define            __AMETHYST__ONB_HPP__

/*
  onb.hpp -- A class to represent an ortho-normal basis
*/

/*
  Revision History:
  18May2001 Created this file.  Wrote constructors, accessors, and functions to
  convert to/from the ONB.
  18Sep2003 Set a namespace, renamed the file (hpp instead of h), added another
            constructor, changed the internal types to be vector3 instead of
            coord3.  
*/

#include "vector3.hpp"
#include "vector_utils.hpp"

namespace amethyst
{
  template <class T>
  class onb
  {
  public:
    // An ONB where only the direction of w matters...
    inline onb(const vector3<T>& a):
      w(unit(a))
    {
      v = perp_vector(w);
      u = crossprod(v,w);
    }
  
    // An ONB where the direction of u,v, and w matter...
    // a and b define a plane, where a is in the direction of u.
    inline onb(const vector3<T>& a, const vector3<T>& b):
      v(unit(b)),
      w(unit(a))
    {
      u = crossprod(v,w);
    }

    // An ONB where the direction of u,v, and w are fully specified.
    inline onb(const vector3<T>& a, const vector3<T>& b, const vector3<T>& c):
      u(unit(a)),
      v(unit(b)),
      w(unit(c))
    {
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
      vector3<T> cv(c);
      return coord3<T>(dotprod(cv,u),dotprod(cv,v),dotprod(cv,w));
    }
    // Take an internal coord, and convert to one for a 'global' ONB.
    inline coord3<T> coord_from_onb(const coord3<T>& c) const
    {
      vector3<T> cv(u * c.x() + v * c.y() + w * c.z());
      return( coord3<T>(cv.x(), cv.y(), cv.z()) );
    }
  
    // Take an external vector, and convert to one for this ONB
    inline vector3<T> vector_to_onb(const vector3<T>& c) const
    {
      return vector3<T>(dotprod(c,u),dotprod(c,v),dotprod(c,w));
    }
    // Take an internal vector, and convert to one for a 'global' ONB.
    inline vector3<T> vector_from_onb(const vector3<T>& c) const
    {
      return (u * c.x() + v * c.y() + w * c.z());
    }

    inline const vector3<T>& get_u() const { return u; }
    inline const vector3<T>& get_v() const { return v; }
    inline const vector3<T>& get_w() const { return w; }
  
    inline vector3<T>& get_u() { return u; }
    inline vector3<T>& get_v() { return v; }
    inline vector3<T>& get_w() { return w; }    
  
  private:
    vector3<T> u, v, w;
  };

} // namespace amethyst
  
#endif /* !defined(__AMETHYST__ONB_HPP__) */