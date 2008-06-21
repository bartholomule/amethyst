/*
 * $Id: dynamic_sphere.hpp,v 1.2 2008/06/21 22:25:10 kpharris Exp $
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

#if       !defined(AMETHYST__DYNAMIC_SPHERE_HPP)
#define            AMETHYST__DYNAMIC_SPHERE_HPP

// --------------------------------------
// Default include of parent class header
// --------------------------------------
#include "amethyst/graphics/shape.hpp"
#include "amethyst/graphics/interpolated_value.hpp"
#include "amethyst/general/defines.hpp"
#include "amethyst/general/rc_pointer.hpp"
#include "amethyst/general/string_format.hpp"

namespace amethyst
{

  /**
   *
   * A dynamic_sphere class.
   *
   * @author Kevin Harris <kpharris@users.sourceforge.net>
   * @version $Revision: 1.2 $
   *
   */
  template<class T>
  class dynamic_sphere : public shape<T>
  {

  private:

    rc_pointer<interpolated_value<T, coord3<T> > > center;
    rc_pointer<interpolated_value<T, T> > radius;

  protected:

  public:
    /** Default constructor */
    dynamic_sphere();

    /** Secondary (sized) constructor */
    dynamic_sphere(const point3<T>& c, T rad);

    dynamic_sphere(const point3<T>& c1, T t1,
		   const point3<T>& c2, T t2,
		   T rad1, T tr1,
		   T rad2, T tr2);

    /** Destructor */
    virtual ~dynamic_sphere();

    /** Copy constructor */
    dynamic_sphere(const dynamic_sphere& old);

    /** Assignment operator */
    dynamic_sphere& operator= (const dynamic_sphere& old);


    point3<T> get_center(T time) const
    {
      return point3<T>(center->interpolate(time));
    }
    T get_radius(T time) const
    {
      return radius->interpolate(time);
    }

    /** Returns if the given point is inside the dynamic_sphere. */
    virtual bool inside(const point3<T>& p) const;

    /** Returns if the given dynamic_sphere intersects the dynamic_sphere. */
    virtual bool intersects(const sphere<T>& s) const;

    /** Returns if the given plane intersects the shape. */
    virtual bool intersects(const plane<T>& p) const;

    /** Returns if the given line intersects the dynamic_sphere. */
    //    virtual bool intersects_line(const line3<T>& line, intersection_info<T>& intersection) const;

    /** Returns if the given line intersects the dynamic_sphere. */
    virtual bool intersects_line(const unit_line3<T>& line,
                                 intersection_info<T>& intersection,
				 const intersection_requirements& requirements) const;

    /**
     * A quick intersection test.  This will calculate nothing but the
     * distance. This is most useful for shadow tests, and other tests where no
     * textures will be applied.
     */
    virtual bool quick_intersection(const unit_line3<T>& line,
				    T time, T& distance) const;

    /**
     * Returns if the given ray intersects the shape.
     *
     */
    virtual bool intersects_ray(const ray_parameters<T>& ray,
				intersection_info<T>& intersection,
				const intersection_requirements& requirements = intersection_requirements()) const;

    virtual std::string internal_members(const std::string& indentation, bool prefix_with_classname = false) const;

    virtual std::string name() const { return "dynamic_sphere"; }

    virtual intersection_capabilities get_intersection_capabilities() const;
    virtual object_capabilities get_object_capabilities() const;

	private:
		 coord2<T> get_uv(const point3<T>& location, T time) const;
  }; // class dynamic_sphere



  //-------------------------------------
  // Default constructor for class dynamic_sphere
  //-------------------------------------
  template<class T>
  dynamic_sphere<T>::dynamic_sphere():
    shape<T>(),
    center(create_interpolation<T,point3<T> >(vector3<T>(0,0,0),point3<T>(0,0,0)).clone_new()),
    radius(create_interpolation<T,T>(T(1), T(1)).clone_new())
  {

  } // dynamic_sphere()

  //----------------------------------------
  // Secondary constructor for class dynamic_sphere
  //----------------------------------------
  template<class T>
  dynamic_sphere<T>::dynamic_sphere(const point3<T>& c, T rad):
    shape<T>(),
    center(create_interpolation<T,point3<T> >(c,c).clone_new()),
    radius(create_interpolation<T,T>(rad,rad).clone_new())
  {

  } // dynamic_sphere(point3,T)

  template<class T>
  dynamic_sphere<T>::dynamic_sphere(const point3<T>& c1, T t1,
				    const point3<T>& c2, T t2,
				    T rad1, T tr1,
				    T rad2, T tr2) :
    shape<T>()
  {
    typedef interpolation_point<T,coord3<T> > center_point_type;

    quick_vector<center_point_type> points(2);
    points[0] = center_point_type(t1,coord3<T>(c1.x(), c1.y(), c1.z()));
    points[1] = center_point_type(t2,coord3<T>(c2.x(), c2.y(), c2.z()));
    center = rc_pointer<interpolated_value<T, coord3<T> > >(create_interpolation<T,coord3<T> >(points).clone_new());


    typedef interpolation_point<T,T> radius_type;
    quick_vector<radius_type> rads(2);
    rads[0] = radius_type(tr1,rad1);
    rads[1] = radius_type(tr2,rad2);
    radius = rc_pointer<interpolated_value<T, T> >(create_interpolation<T,T>(rads).clone_new());
  } // dynamic_sphere(point3,T,point3,T,T,T,T,T)

  //----------------------------
  // Destructor for class dynamic_sphere
  //----------------------------
  template<class T>
  dynamic_sphere<T>::~dynamic_sphere()
  {

  } // ~dynamic_sphere()

  //----------------------------------
  // Copy constructor for class dynamic_sphere
  //----------------------------------
  template<class T>
  dynamic_sphere<T>::dynamic_sphere(const dynamic_sphere<T>& old):
    shape<T>(old),
    center(old.center),
    radius(old.radius)
  {

  } // dynamic_sphere(dynamic_sphere)

  //-------------------------------------
  // Assignment operator for class dynamic_sphere
  //-------------------------------------
  template<class T>
  dynamic_sphere<T>& dynamic_sphere<T>::operator= (const dynamic_sphere<T>& old)
  {
    // Generic check for self-assignment
    if( &old != this )
    {
      center = old.center;
      radius = old.radius;

      shape<T>::operator=(old);
    }
    return(*this);
  } // dynamic_sphere::operator=(dynamic_sphere)


  // Returns if the given point is inside the dynamic_sphere.
  template<class T>
  bool dynamic_sphere<T>::inside(const point3<T>& p) const
  {
    // FIXME! Check the entire range

    // The epsilon adjusted radius is (r^2 + 2*r*E + E^2)
    T my_radius = get_radius(T(0));
    return( squared_length(p - get_center(T(0))) <
            (my_radius * my_radius +
             2 * my_radius * AMETHYST_EPSILON +
             AMETHYST_EPSILON * AMETHYST_EPSILON) );
  }

  // Returns if the given dynamic_sphere intersects the dynamic_sphere.
  template<class T>
  bool dynamic_sphere<T>::intersects(const sphere<T>& s) const
  {
    T my_radius = get_radius(T(0));
    point3<T> my_center = get_center(T(0));

    T combined_radius = s.get_radius() + my_radius + AMETHYST_EPSILON;
    return( squared_length(s.get_center() - my_center) <
            (combined_radius * combined_radius) );
  }

  // Returns if the given plane intersects the shape.
  template<class T>
  bool dynamic_sphere<T>::intersects(const plane<T>& p) const
  {
    // FIXME! Check the entire range, do something better.
    sphere<T> s(get_center(T(0)),
		get_radius(T(0)));

    return amethyst::intersects(s, p);
  }

  // Returns if the given line intersects the dynamic_sphere.
  template <class T>
  bool dynamic_sphere<T>::intersects_line(const unit_line3<T>& line,
					  intersection_info<T>& intersection,
					  const intersection_requirements& requirements) const
  {
	  // FIXME! Check the entire time range, not just 0....
	  T hit_distance;
		T time = T(0);
	  if( quick_intersection(line, time, hit_distance) )
	  {
		  intersection.set_shape(this);
		  intersection.set_first_distance(hit_distance);
		  intersection.set_first_point( line.point_at(hit_distance) );
		  intersection.set_ray(line);

		  if( requirements.needs_normal() )
		  {
				 intersection.set_normal(unit(intersection.get_first_point() - get_center(time)));
		  }

			if( requirements.needs_uv() )
			{
				 intersection.set_uv(get_uv(intersection.get_first_point(), time));
			}

		  // FIXME! Follow the rest of the requirements
			// FIXME! multiple intersections if needed.
		  return true;
	  }
	  return false;
  }

  /**
   * A quick intersection test.  This will calculate nothing but the
   * distance. This is most useful for shadow tests, and other tests where no
   * textures will be applied.
   */
  template <class T>
  bool dynamic_sphere<T>::quick_intersection(const unit_line3<T>& line,
					     T time, T& distance) const
  {
		 // FIXME! Check the entire range
		 T my_radius = get_radius(time);
		 point3<T> my_center = get_center(time);

		 // This is the fast hit test.
		 vector3<T> o_c = line.origin() - my_center;
		 register T A = dotprod(line.direction(), line.direction());
		 register T B = 2 * dotprod(line.direction(), o_c);
		 register T C = dotprod(o_c, o_c) - my_radius * my_radius;
		 T discriminant = B * B - 4 * A * C;

		 if( discriminant >= AMETHYST_EPSILON )
		 {
				T sqrtd = sqrt(discriminant);

				// If t1 is inside, it MUST be the closest, as A will always be positive
				// (squared length of the line), and the subtraction will be less than
				// the addition (as the square root will always be positive).
				T t1 = (-B - sqrtd) / (2 * A);
				if( line.inside(t1) )
				{
					 distance = t1;
					 return true;
				}
				else
				{
					 // The first side (although a hit), wasn't inside the range.
					 T t2 = (-B + sqrtd) / (2 * A);
					 if( line.inside(t2) )
					 {
							distance = t2;
							return true;
					 }
				}
		 }
		 return false;
  }

  template <class T>
  bool dynamic_sphere<T>::intersects_ray(const ray_parameters<T>& ray,
					intersection_info<T>& intersection,
					const intersection_requirements& requirements) const
  {
     T hit_distance;
     if( quick_intersection(ray.get_line(), ray.get_time(), hit_distance) )
     {
				const unit_line3<T>& line = ray.get_line();
        intersection.set_shape(this);
        intersection.set_first_distance(hit_distance);
        intersection.set_first_point( line.point_at(hit_distance) );
        intersection.set_ray(line);

        if( requirements.needs_normal() )
        {
           intersection.set_normal(unit(intersection.get_first_point() - get_center(ray.get_time())));
        }

				if( requirements.needs_uv() )
				{
					 intersection.set_uv(get_uv(intersection.get_first_point(), ray.get_time()));
				}

				// FIXME! multiple intersections

        return true;
     }
     return false;
  }

  template <class T>
  std::string dynamic_sphere<T>::internal_members(const std::string& indentation, bool prefix_with_classname) const
  {
    std::string retval;
    std::string internal_tagging = indentation;

    if( prefix_with_classname )
    {
      internal_tagging += dynamic_sphere<T>::name() + "::";
    }

    retval += indentation + string_format("intersection_capabilities=%1\n", get_intersection_capabilities().to_string());
    retval += indentation + string_format("object_capabilities=%1\n", get_object_capabilities().to_string());
    retval += internal_tagging + string_format("center=%1\n", center);
    retval += internal_tagging + string_format("radius=%1\n", radius);

    return retval;
  }

  template <class T>
  intersection_capabilities dynamic_sphere<T>::get_intersection_capabilities() const
  {
    intersection_capabilities caps = shape<T>::get_intersection_capabilities();

    caps |= intersection_capabilities::HIT_FIRST;
    caps |= intersection_capabilities::HIT_ALL;
    caps |= intersection_capabilities::NORMAL_CALCULATION;
    caps |= intersection_capabilities::UV_CALCULATION;
    //    caps |= intersection_capabilities::LOCAL_SYSTEM_CALCULATION
    return caps;
  }

  template <class T>
  object_capabilities dynamic_sphere<T>::get_object_capabilities() const
  {
    object_capabilities caps = shape<T>::get_object_capabilities();

    caps |= object_capabilities::BOUNDABLE;
    caps |= object_capabilities::SIMPLE;

    return caps;
  }

	template <class T>
	coord2<T> dynamic_sphere<T>::get_uv(const point3<T>& location, T time) const
	{
		 vector3<T> point_vector = location - get_center(time);

		 T cos_theta = point_vector.y() / get_radius(time);
		 // phi = [-pi,pi]
		 // theta = [0,pi]
		 T theta = acos(cos_theta);
		 T phi = atan2(point_vector.z(), point_vector.x());
		 T u = (phi + M_PI) / (2 * M_PI);
		 T v = theta / M_PI;

		 return coord2<T>(u, v);
	}

} // namespace amethyst


#endif /* !defined(AMETHYST__DYNAMIC_SPHERE_HPP) */
