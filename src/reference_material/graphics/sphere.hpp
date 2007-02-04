/*
 * $Id: sphere.hpp,v 1.9 2007/02/04 06:18:28 kpharris Exp $
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

#if       !defined(AMETHYST__SPHERE_HPP)
#define            AMETHYST__SPHERE_HPP

// --------------------------------------
// Default include of parent class header
// --------------------------------------
#include "shape.hpp"
#include <general/defines.hpp>
#include <general/string_format.hpp>

namespace amethyst
{

	/**
	 *
	 * A sphere class.
	 *
	 * @author Kevin Harris <kpharris@users.sourceforge.net>
	 * @version $Revision: 1.9 $
	 *
	 */
	template<class T>
	class sphere : public shape<T>
	{

	private:

		point3<T> center;
		T radius;
		T radius_squared;

	protected:

	public:
		/** Default constructor */
		sphere();

		/** Secondary (sized) constructor */
		sphere(const point3<T>& c, T rad);

		/** Destructor */
		virtual ~sphere();

		/** Copy constructor */
		sphere(const sphere& old);

		/** Assignment operator */
		sphere& operator= (const sphere& old);


		point3<T> get_center() const
		{
			return center;
		}
		T get_radius() const
		{
			return radius;
		}

		/** Returns if the given point is inside the sphere. */
		virtual bool inside(const point3<T>& p) const;

		/** Returns if the given sphere intersects the sphere. */
		virtual bool intersects(const sphere<T>& s) const;

		/** Returns if the given plane intersects the shape. */
		virtual bool intersects(const plane<T>& p) const;

		/** Returns if the given line intersects the sphere. */
		//    virtual bool intersects_line(const line3<T>& line, intersection_info<T>& intersection) const;

		/** Returns if the given line intersects the sphere. */
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

		virtual std::string internal_members(const std::string& indentation, bool prefix_with_classname = false) const;

		virtual std::string to_string(const std::string& base_indentation,
			const std::string& level_indentation = "  ") const;

		virtual std::string name() const
		{
			return "sphere";
		}

		virtual intersection_capabilities get_intersection_capabilities() const;
		virtual object_capabilities get_object_capabilities() const;
	}; // class sphere



	//-------------------------------------
	// Default constructor for class sphere
	//-------------------------------------
	template<class T>
	sphere<T>::sphere():
		shape<T>(),
		center(0,0,0),
		radius(0),
		radius_squared(0)
	{

	} // sphere()

	//----------------------------------------
	// Secondary constructor for class sphere
	//----------------------------------------
	template<class T>
	sphere<T>::sphere(const point3<T>& c, T rad):
		shape<T>(),
		center(c),
		radius(rad),
		radius_squared(rad * rad)
	{
		if( radius < T(0) )
		{
			radius = -radius;
		}
	} // sphere(point3,T)

	//----------------------------
	// Destructor for class sphere
	//----------------------------
	template<class T>
	sphere<T>::~sphere()
	{

	} // ~sphere()

	//----------------------------------
	// Copy constructor for class sphere
	//----------------------------------
	template<class T>
	sphere<T>::sphere(const sphere<T>& old):
		shape<T>(old),
		center(old.center),
		radius(old.radius),
		radius_squared(old.radius_squared)
	{

	} // sphere(sphere)

	//-------------------------------------
	// Assignment operator for class sphere
	//-------------------------------------
	template<class T>
	sphere<T>& sphere<T>::operator= (const sphere<T>& old)
	{
		// Generic check for self-assignment
		if( &old != this )
		{
			center = old.center;
			radius = old.radius;
			radius_squared = old.radius_squared;

			shape<T>::operator=(old);
		}
		return(*this);
	} // sphere::operator=(sphere)


	// Returns if the given point is inside the sphere.
	template<class T>
	bool sphere<T>::inside(const point3<T>& p) const
	{
		// The epsilon adjusted radius is (r^2 + 2*r*E + E^2)
		return( squared_length(p - center) <
			(radius_squared +
				2 * radius * AMETHYST_EPSILON +
				AMETHYST_EPSILON * AMETHYST_EPSILON) );
	}

	// Returns if the given sphere intersects the sphere.
	template<class T>
	bool sphere<T>::intersects(const sphere<T>& s) const
	{
		T combined_radius = s.radius + radius + AMETHYST_EPSILON;
		return( squared_length(s.center - center) <
			(combined_radius * combined_radius) );
	}

	// Returns if the given plane intersects the shape.
	template<class T>
	bool sphere<T>::intersects(const plane<T>& p) const
	{
		return amethyst::intersects(*this, p);
	}

	// Returns if the given line intersects the sphere.
	template <class T>
	bool sphere<T>::intersects_line(const unit_line3<T>& line,
		intersection_info<T>& intersection,
		const intersection_requirements& requirements) const
	{
		// FIXME! We need to allow multiple hits.
		// This is the fast hit test, it should be kept fairly fast while allowing
		// multiple hits.
		vector3<T> o_c = line.origin() - center;
		register T A = dotprod(line.direction(), line.direction());
		register T B = 2 * dotprod(line.direction(), o_c);
		register T C = dotprod(o_c, o_c) - radius_squared;
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
				intersection.set_shape(this);
				intersection.set_first_distance(t1);
				intersection.set_first_point( line.point_at(t1) );
				intersection.set_ray(line);


				if( requirements.needs_normal() )
				{
					intersection.set_normal(unit(intersection.get_first_point() - center));
				}

				// FIXME! Follow the requirements


				return true;
			}
			else
			{
				// The first side (although a hit), wasn't inside the range.
				T t2 = (-B + sqrtd) / (2 * A);
				if( line.inside(t2) )
				{
					intersection.set_shape(this);
					intersection.set_first_distance(t2);
					intersection.set_first_point( line.point_at(t2) );
					intersection.set_ray(line);

					if( requirements.needs_normal() )
					{
						intersection.set_normal(unit(intersection.get_first_point() - center));
					}

					// FIXME! Follow the requirements (uv, local coords, etc).
					return true;
				}
			}
		}
		return false;
	}

	/**
	 * A quick intersection test.  This will calculate nothing but the
	 * distance. This is most useful for shadow tests, and other tests where no
	 * textures will be applied.
	 */
	template <class T>
	bool sphere<T>::quick_intersection(const unit_line3<T>& line,
		T time, T& distance) const
	{
		(void) time; // not a moving or resizing sphere...
		// This is the fast hit test.
		vector3<T> o_c = line.origin() - center;
		register T A = dotprod(line.direction(), line.direction());
		register T B = 2 * dotprod(line.direction(), o_c);
		register T C = dotprod(o_c, o_c) - radius_squared;
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
	std::string sphere<T>::internal_members(const std::string& indentation, bool prefix_with_classname) const
	{
		std::string retval;
		std::string internal_tagging = indentation;

		if( prefix_with_classname )
		{
			internal_tagging += sphere<T>::name() + "::";
		}

		retval += indentation + string_format("intersection_capabilities=%1\n", get_intersection_capabilities().to_string());
		retval += indentation + string_format("object_capabilities=%1\n", get_object_capabilities().to_string());
		retval += internal_tagging + string_format("center=%1\n", center);
		retval += internal_tagging + string_format("radius=%1\n", radius);

		return retval;
	}

	template <class T>
	std::string sphere<T>::to_string(const std::string& indent,
		const std::string& level_indent) const
	{
		return ( indent + "sphere\n" +
			indent + "{\n" +
			sphere<T>::internal_members(indent + level_indent, false) +
			indent + "}" );
	}

	template <class T>
	intersection_capabilities sphere<T>::get_intersection_capabilities() const
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
	object_capabilities sphere<T>::get_object_capabilities() const
	{
		object_capabilities caps = shape<T>::get_object_capabilities();

		caps |= object_capabilities::BOUNDABLE;
		caps |= object_capabilities::SIMPLE;

		return caps;
	}

} // namespace amethyst


#endif /* !defined(AMETHYST__SPHERE_HPP) */
