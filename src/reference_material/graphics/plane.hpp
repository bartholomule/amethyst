/*
 * $Id: plane.hpp,v 1.8 2004/04/07 05:10:05 kpharris Exp $
 *
 * Part of "Amethyst" a playground for graphics development
 * Copyright (C) 2004 Kevin Harris
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

#if       !defined(AMETHYST__PLANE_HPP)
#define            AMETHYST__PLANE_HPP

// --------------------------------------
// Default include of parent class header
// --------------------------------------
#include "shape.hpp"
#include "sphere.hpp"
#include <math/coord2.hpp>
#include <general/string_format.hpp>

namespace amethyst
{ 

  /**
   * 
   * A plane in 3d.
   * 
   * @author Kevin Harris <kpharris@users.sourceforge.net>
   * @version $Revision: 1.8 $
   * 
   */
  template<class T>
  class plane : public shape<T>
  {

  private:

    point3<T> defining_point;
    vector3<T> normal;
    vector3<T> u_vector;
    vector3<T> v_vector;
    int non_zero_u_index;
    int non_zero_v_index;

  protected:

  public:
    /** Default constructor */
    plane();

    /** Secondary (pos/orientation) constructor */
    plane(const point3<T>& p, const vector3<T>& n);

    /** Tertiary (pos/orientation/rotation) constructor */
    plane(const point3<T>& p, const vector3<T>& n, const vector3<T>& vec_u);

    /** Quaternary (pos1, pos2, pos3) constructor */
    plane(const point3<T>& p, const point3<T>& plus_u, const point3<T>& plus_v);
    /** Quinary (pos, vec, vec, vec) constructor */
    plane(const point3<T>& p, const vector3<T>& normal, const vector3<T>& vec_u, const vector3<T>& vec_v);

    /** Destructor */
    virtual ~plane();

    /** Copy constructor */
    plane(const plane& old);

    /** Assignment operator */
    plane& operator= (const plane& old);

    const point3<T>& get_origin() const
    {
      return defining_point;
    }
    const vector3<T>& get_normal() const
    {
      return normal;
    }
    const vector3<T>& get_u_vector() const
    {
      return u_vector;
    }
    const vector3<T>& get_v_vector() const
    {
      return v_vector;
    }

    /** Returns if the given point is inside the shape. */
    virtual bool inside(const point3<T>& p) const;

    /** Returns if the given sphere intersects the shape. */
    virtual bool intersects(const sphere<T>& s) const;

    /** Returns if the given plane intersects the shape. */
    virtual bool intersects(const plane<T>& p) const;

    /** Returns if the given line intersects the sphere. */
    virtual bool intersects_line(const line3<T>& line, intersection_info<T>& intersection) const;

    /** Returns if the given line intersects the plane. */
    virtual bool intersects_line(const unit_line3<T>& line,
                                 intersection_info<T>& intersection) const;

    virtual std::string internal_members(const std::string& indentation, bool prefix_with_classname = false) const;

    virtual std::string to_string(const std::string& base_indentation,
                                  const std::string& level_indentation = "  ") const;

    virtual std::string name() const
    {
      return "plane";
    }

    bool extract_uv_for_point(const point3<T>& point, coord2<T>& uv) const;

  private:
    void setup_non_zero_indices();

  }; // class plane



  //------------------------------------
  // Default constructor for class plane
  //------------------------------------
  template<class T>
  plane<T>::plane():
    shape<T>(),
    defining_point(0,0,0),
    normal(0,0,0),
    u_vector(0,0,0),
    v_vector(0,0,0),
    non_zero_u_index(0),
    non_zero_v_index(0)
  {

  } // plane()

  //---------------------------------------------------------
  // Secondary (pos/orientation) constructor for class plane.
  //---------------------------------------------------------
  template <class T>
  plane<T>::plane(const point3<T>& p, const vector3<T>& n):
    shape<T>(),
    defining_point(p),
    normal(unit(n))
  {
    v_vector = unit(crossprod(normal,
                              point3<T>(defining_point.x() + 1,
                                        defining_point.y(),
                                        defining_point.z()) -
                              defining_point));
    u_vector = crossprod(v_vector, normal);

    setup_non_zero_indices();
  } // plane(point3,vector3)

  //---------------------------------------------------------
  // Tertiary (pos/orientation/rotation) constructor for class plane.
  //---------------------------------------------------------
  template <class T>
  plane<T>::plane(const point3<T>& p, const vector3<T>& n, const vector3<T>& vec_u):
    shape<T>(),
    defining_point(p),
    normal(unit(n)),
    v_vector(crossprod(normal, vec_u))
  {
    u_vector = crossprod(v_vector, normal);

    setup_non_zero_indices();

  } // plane(point3,vector3,vector3)

  //---------------------------------------------------------
  // Quaternary (pos1, pos2, pos3) constructor for class plane.
  //---------------------------------------------------------
  template <class T>
  plane<T>::plane(const point3<T>& p, const point3<T>& plus_u, const point3<T>& plus_v):
    shape<T>(),
    defining_point(p)
  {
    u_vector = plus_u - p;
    v_vector = plus_v - p;
    normal = unit(crossprod(u_vector, v_vector));

    setup_non_zero_indices();

  } // plane(point3,point3,point3)    

  //---------------------------------------------------------
  // Quinary (pos, vec, vec, vec) constructor for class plane.
  //---------------------------------------------------------
  template <class T>
  plane<T>::plane(const point3<T>& p, const vector3<T>& normal,
                  const vector3<T>& vec_u, const vector3<T>& vec_v):
    shape<T>(),
    defining_point(p),
    normal(unit(crossprod(vec_u, vec_v))),
    u_vector(vec_u),
    v_vector(vec_v)
  {

    setup_non_zero_indices();

  } // plane(point3,vector3,vector3,vector3)

  //---------------------------
  // Destructor for class plane
  //---------------------------
  template<class T>
  plane<T>::~plane()
  {

  } // ~plane()

  //---------------------------------
  // Copy constructor for class plane
  //---------------------------------
  template<class T>
  plane<T>::plane(const plane<T>& old):
    shape<T>(old),
    defining_point(old.defining_point),
    normal(old.normal),
    u_vector(old.u_vector),
    v_vector(old.v_vector),
    non_zero_u_index(old.non_zero_u_index),
    non_zero_v_index(old.non_zero_v_index)
  {

  } // plane(plane)

  //------------------------------------
  // Assignment operator for class plane
  //------------------------------------
  template<class T>
  plane<T>& plane<T>::operator= (const plane<T>& old)
  {
    // Generic check for self-assignment
    if( &old != this )
    {
      defining_point = old.defining_point;
      normal = old.normal;
      u_vector = old.u_vector;
      v_vector = old.v_vector;

      non_zero_u_index = old.non_zero_u_index;
      non_zero_v_index = old.non_zero_v_index;
      shape<T>::operator=(old);
    }
    return(*this);
  } // plane::operator=(plane)

  // Returns if the given point is inside the shape.
  template <class T>
  inline bool plane<T>::inside(const point3<T>& p) const
  {
    T dist = dotprod(p - defining_point, normal);
    return(dist < AMETHYST_EPSILON) && (dist > -AMETHYST_EPSILON);
  }

  // Returns if the given sphere intersects the shape.
  template <class T>
  inline bool plane<T>::intersects(const sphere<T>& s) const
  {
    T dist = dotprod(s.get_center() - defining_point, normal);
    T max_dist = s.get_radius() + AMETHYST_EPSILON;
    return(dist < max_dist) && (dist > -max_dist);
  }

  // Returns if the given plane intersects the shape.
  template <class T>
  inline bool plane<T>::intersects(const plane<T>& p) const
  {
    T normal_proj = dotprod(normal, p.normal);

    // Flip it if required.
    if( normal_proj < T(0) )
    {
      normal_proj = -normal_proj;
    }

    // If they are not parallel, then they intersect somewhere.
    if( normal_proj < (1 - AMETHYST_EPSILON) )
    {
      return true;
    }
    // If they are parallel, but have the same defining point, then they
    // overlap EVERYWHERE (the same plane).
    else if( squared_length(defining_point - p.defining_point) <
             AMETHYST_EPSILON )
    {
      return true;
    }
    // Anything else 
    else
    {
      return false;
    }
  }


  // Returns if the given line intersects the plane.
  template <class T>
  inline bool plane<T>::intersects_line(const unit_line3<T>& line,
                                        intersection_info<T>& intersection) const
  {
    T ctheta = dotprod(line.direction(), normal);
    T t;

    // I will need to explain why this is safe to do.  If the angle is > 0 or
    // the angle is < 0, then we know there is a hit somewhere, and it can be
    // found.  If the angle is zero, then it will cause a divide by zero,
    // setting t to be +/-INF, which when compared to anything (even itself)
    // returns false, thus, the inside test below can be called with the INF,
    // and not get bad numbers out.
    //
    // DON'T DELETE THESE COMMENTS, AS THEY APPLY TO OTHER PLANE-BASED
    // FUNCTIONS AS WELL!
    if( ctheta > 0 )
    {
      t = (dotprod(defining_point - line.origin(), normal) /
           ctheta);
    }
    else
    {
      t = (dotprod(defining_point - line.origin(), -normal) /
           -ctheta);
    }

    if( line.inside(t) )
    {
      intersection.set(this, t);
      return true;
    }

    return false;
  }

  // Returns if the given line intersects the plane.
  template <class T>
  inline bool plane<T>::intersects_line(const line3<T>& line,
                                        intersection_info<T>& intersection) const
  {
    T ctheta = ( dotprod(line.direction(), normal) /
                 length(line.direction()) );
    T t;

    // Check the unit line version of this function for proper comments on why
    // this works!
    if( ctheta > 0 )
    {
      t = (dotprod(defining_point - line.origin(), normal) /
           ctheta);
    }
    else
    {
      t = (dotprod(defining_point - line.origin(), -normal) /
           -ctheta);
    }

    if( line.inside(t) )
    {
      intersection.set(this, t);
      return true;
    }

    return false;
  }

  template <class T>
  std::string plane<T>::internal_members(const std::string& indentation, bool prefix_with_classname) const
  {
    std::string retval;
    std::string internal_tagging = indentation;

    if( prefix_with_classname )
    {
      internal_tagging += plane<T>::name() + "::";
    }

    retval += internal_tagging + string_format("point=%1\n", defining_point);
    retval += internal_tagging + string_format("normal=%1\n", normal);
    retval += internal_tagging + string_format("u=%1\n", u_vector);
    retval += internal_tagging + string_format("v=%1\n", v_vector);
    retval += internal_tagging + string_format("nz_indices=(%1,%2)\n",
                                               non_zero_u_index,
                                               non_zero_v_index);
    return retval;
  }

  template <class T>
  std::string plane<T>::to_string(const std::string& indent,
                                  const std::string& level_indent) const
  {
    return ( indent + "plane\n" +
             indent + "{\n" +
             plane<T>::internal_members(indent + level_indent, false) +
             indent + "}" );         
  }

  template <class T>
  void plane<T>::setup_non_zero_indices()
  {
    for( non_zero_u_index = 0; non_zero_u_index < 3; ++non_zero_u_index )
    {
      if(fabs(u_vector[non_zero_u_index]) > AMETHYST_EPSILON)
        break;
    }
    for( non_zero_v_index = 0; non_zero_v_index < 3; ++non_zero_v_index )
    {
      if(non_zero_v_index == non_zero_u_index) continue;
      if(fabs(v_vector[non_zero_v_index]) > AMETHYST_EPSILON)
        break;
    }
  }

  template<class T>
  inline bool plane<T>::extract_uv_for_point(const point3<T>& point,
                                             coord2<T>& uv) const
  {
    if( plane<T>::inside(point) )
    {
      T u;
      T v;
      vector3<T> point_diff_vector(point - defining_point);
      T u_scalar = u_vector[non_zero_v_index] / u_vector[non_zero_u_index];
      v = ( ( point_diff_vector[non_zero_v_index] - 
              point_diff_vector[non_zero_u_index] * u_scalar ) /
            ( v_vector[non_zero_v_index] - 
              v_vector[non_zero_u_index] * u_scalar) );
      u = ( ( point_diff_vector[non_zero_u_index]  - 
              v * v_vector[non_zero_u_index]) /
            u_vector[non_zero_u_index] );

      uv.set(u,v);

      return true;
    }
    return false;
  }


  template <class T>
  bool intersects(const sphere<T>& s, const plane<T>& p)
  {
    return p.intersects(s);
  }


} // namespace amethyst


#endif /* !defined(AMETHYST__PLANE_HPP) */
