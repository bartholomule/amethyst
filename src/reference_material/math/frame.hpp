/*
 * $Id: frame.hpp,v 1.2 2004/02/14 06:55:18 kpharris Exp $
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
#if       !defined(AMETHYST__FRAME_HPP)
#define            AMETHYST__FRAME_HPP

/*
  frame.hpp -- A class to represent a complete coordinate system.

  Defining a frame with the u,v,w axii (think of them as x,y,z renamed
  to avoid any confusion) and the origin, will allow transformation
  from the global frame into that frame (transform), and from
  that frame into the global frame (inverse_transform).

  All transformations are simple rotations and translations.  No skew,
  scale, or any other such nasties can be performed.
*/

/*
  Revision History:
  18May2001 Created this file.
  18Sep2003 Renamed this file--(frame.hpp instead of frame.h), added
            the namespace, removed the matrix (as I don't think it is
            needed for simple coordinate system changes).  Also most
            things were rewritten to use vectors and points instead of
            coords. 
*/

#include "onb.hpp"

namespace amethyst
{
  template <class T>
  class frame
  {
  public:
    inline frame(const point3<T>& origin, const vector3<T>& w):
      my_basis(w),
      my_origin(origin)
    {
    }
    inline frame(const point3<T>& origin,
		 const vector3<T>& a, const vector3<T>& b):
      my_basis(a,b),
      my_origin(origin)
    {
    }

    inline frame(const point3<T>& origin,
		 const vector3<T>& a, const vector3<T>& b, const vector3<T>& c):
      my_basis(a,b,c),
      my_origin(origin)
    {
    }  

    inline frame(const frame<T>& f):
      my_basis(f.my_basis),
      my_origin(f.my_origin)
    {
    }
    inline frame<T>& operator=(const frame<T>& f)
    {
      if(&f != this)
      {
	my_basis = f.my_basis;
	my_origin = f.my_origin;
      }
      return(*this);
    }

    inline vector3<T> u() const { return my_basis.get_u(); }
    inline vector3<T> v() const { return my_basis.get_v(); }
    inline vector3<T> w() const { return my_basis.get_w(); }
    inline point3<T>  o() const { return my_origin; }

    onb<T>&    basis()  { return my_basis; }
    point3<T>& origin() { return my_origin; }  
    const onb<T>&    basis()  const { return my_basis; }
    const point3<T>& origin() const { return my_origin; }  

    //
    // Transfrom from external frame INTO this frame.
    //
    inline vector3<T> transform(const vector3<T>& v) const
    {
      return my_basis.into_onb(v);
    }

    inline point3<T> transform(const point3<T>& p) const
    {
      vector3<T> v = p - origin();
      return my_basis.into_onb(point3<T>(v.x(),v.y(),v.z()));
    }

    inline coord3<T> transform(const coord3<T>& c) const
    {
      coord3<T> c2(my_origin.x(), my_origin.y(), my_origin.z());
      return my_basis.into_onb(c - c2);
    }

    //
    // Transform OUT OF this frame into the 'global' frame.
    //
    inline vector3<T> inverse_transform(const vector3<T>& v) const
    {
      return my_basis.outof_onb(v);
    }
  
    inline point3<T> inverse_transform(const point3<T>& p) const
    {
      return (my_basis.outof_onb(p) +
	      vector3<T>(my_origin.x(), my_origin.y(), my_origin.z()));
    }

    inline coord3<T> inverse_transform(const coord3<T>& c) const
    {
      coord3<T> c2(my_origin.x(), my_origin.y(), my_origin.z());
      return my_basis.outof_onb(c) + c2;
    }    

    //
    // Transfrom from external frame INTO this frame, treating the
    // triplet as requested.
    //
    inline coord3<T> transform_as_point(const coord3<T>& c)
    {
      point3<T> p = transform(point3<T>(c));
      return coord3<T>(p.x(), p.y(), p.z());
    }

    inline coord3<T> transform_as_vector(const coord3<T>& c)
    {
      vector3<T> p = transform(vector3<T>(c));
      return coord3<T>(p.x(), p.y(), p.z());
    }

    inline coord3<T> transform_as_normal(const coord3<T>& c)
    {
      // Normals would only be rotated.  They are NOT scaled, skewed, or
      // anything else.  All we have to do is make sure it is a unit
      // vector.
      return unit(transform_as_vector(c));
    }

    // Transform into the frame (from the global).
    template <class vector_type>
    vector_type transform_in(const vector_type& v)
    {
      return transform(v);
    }

    // Transform from the frame to the global.
    template <class vector_type>
    vector_type transform_out(const vector_type& v)
    {
      return inverse_transform(v);
    }    
    
  private:
    onb<T>    my_basis;
    point3<T> my_origin;
  }; // class frame


  //
  // This function will give the single frame which allows the
  // complete transformation of both frames (applied as inner first,
  // then outer).
  //
  template <class T>
  frame<T> get_combined_frame(const frame<T>& outer_frame,
			      const frame<T>& inner_frame)
  {
    return frame<T>(outer_frame.inverse_transform(inner_frame.o()),
		    outer_frame.inverse_transform(inner_frame.u()),
		    outer_frame.inverse_transform(inner_frame.v()),
		    outer_frame.inverse_transform(inner_frame.w()));
  } // get_combined_frame(frame, frame)


  
} // namespace amethyst



#endif /* !defined(AMETHYST__FRAME_HPP) */
