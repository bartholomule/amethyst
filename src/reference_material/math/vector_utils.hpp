/*
 * $Id: vector_utils.hpp,v 1.2 2004/02/14 06:55:18 kpharris Exp $
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

#if       !defined(AMETHYST__VECTOR_UTILS_HPP)
#define            AMETHYST__VECTOR_UTILS_HPP

#include <algorithm>
#include "../general/template_functions.hpp"

/*
 * The functions in this file take any kind of 3d vector, providing that it supplies 3 things:
 * (1) x(), y(), z() functions (const is ok).
 * (2) [] operator(unsigned) (const is ok)
 * (3) a typedef for its base type as a public member named 'base'.
 */

namespace amethyst
{

  template <class vector_type>
  inline vector_type invert(const vector_type& v)
  {
    return(vector_type(1/v.x(), 1/v.y(), 1/v.z()));
  }
  
  template <class vector_type>
  inline typename vector_type::base average(const vector_type& v)
  {
    return((v.x() + v.y() + v.z())/3);
  }
  
  template <class vector_type>
  inline typename vector_type::base max(const vector_type& v)
  {
    return std::max(v.x(), std::max(v.y(),v.z()));
  }
  
  template <class vector_type>
  vector_type abs_vector(const vector_type& v)
  {
    return vector_type(my_abs(v.x()), my_abs(v.y()), my_abs(v.z()));
  }

  template <class vector_type>
  inline unsigned min_abs_index(const vector_type& v)
  {
    unsigned min_index = 0;
    typename vector_type::base min_abs = my_abs(v[min_index]);
    for(unsigned i = 1; i < 3; ++i)
    {
      if(my_abs(v[i]) < min_abs)
      {
	min_index = i;
	min_abs = my_abs(v[min_index]);
      }
    }
    return(min_index);
  }
  
  template <class vector_type>
  inline vector_type perp_vector(const vector_type& v)
  {
    vector_type temp = unit(v);
    unsigned zero_index = min_abs_index(temp);
    if(zero_index == 0)
    {
      return unit(vector_type(0,v.z(),-v.y()));
    }
    else if( zero_index == 1 )
    {
      return(unit(vector_type(v.z(),0,-v.x())));
    }
    else
    {
      return(unit(vector_type(v.y(),-v.x(),0)));
    }
  } // perp_vector(const vector_type&)

  template <class vector_type> inline bool magnitude_greater(const vector_type& v1, const vector_type& v2)
  {
    return dotprod(v1,v1) > dotprod(v2,v2);
  }
  
  template <class vector_type> inline bool magnitude_less(const vector_type& v1, const vector_type& v2)
  {
    return dotprod(v1,v1) < dotprod(v2,v2);
  }

  // Finds the best planar projection (axii), given the normal of the plane.
  template <class vector_type>
  void best_planar_projection(const vector_type& normal, unsigned& axis1, unsigned& axis2)
  {
    if((my_abs(normal.z()) > my_abs(normal.y())) &&
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
  } // best_planar_projection(const coord3&, unsigned&, unsigned&)

  
} // namespace amethyst  

#endif /* !defined(AMETHYST__VECTOR_UTILS_HPP) */
