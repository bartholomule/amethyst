/*
 * $Id: aggregate.hpp,v 1.10 2008/06/16 10:17:48 kpharris Exp $
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

#if       !defined(AMETHYST__AGGREGATE_HPP)
#define            AMETHYST__AGGREGATE_HPP

// --------------------------------------
// Default include of parent class header
// --------------------------------------
#include "graphics/shape.hpp"
#include "graphics/capabilities.hpp"

#include "general/quick_vector.hpp"
#include "general/rc_pointer.hpp"

namespace amethyst
{

   /**
    *
    * An aggregate (collection) of shapes.
    *
    * @author Kevin Harris <kpharris@users.sourceforge.net>
    * @version $Revision: 1.10 $
    *
    */
   template<class T>
   class aggregate : public shape<T>
   {
   public:
      typedef rc_pointer<shape<T> > shape_pointer_type;
      typedef quick_vector<shape_pointer_type> shape_list_type;

   private:
      shape_list_type shape_list;

   protected:

   public:
      /** Default constructor */
      aggregate();

      /** Destructor */
      virtual ~aggregate();

      /** Copy constructor */
      aggregate(const aggregate& old);

      /** Assignment operator */
      aggregate& operator= (const aggregate& old);

      /** Returns if the given point is inside the shape. */
      virtual bool inside(const point3<T>& p) const;

      /** Returns if the given sphere intersects the shape. */
      virtual bool intersects(const sphere<T>& s) const;

      /** Returns if the given plane intersects the shape. */
      virtual bool intersects(const plane<T>& p) const;

      /** Returns if the given line intersects the shape. */
      virtual bool intersects_line(const unit_line3<T>& line,
         intersection_info<T>& intersection,
         const intersection_requirements& requirements = intersection_requirements()) const;

      virtual bool intersects_line(const line3<T>& line,
         intersection_info<T>& intersection,
         const intersection_requirements& requirements = intersection_requirements()) const;

      virtual bool intersects_ray(const ray_parameters<T>& ray,
         intersection_info<T>& intersection,
         const intersection_requirements& requirements = intersection_requirements()) const;

      /**
       * A quick intersection test.  This will calculate nothing but the
       * distance. This is most useful for shadow tests, and other tests where no
       * textures will be applied.
       */
      virtual bool quick_intersection(const unit_line3<T>& line, T time, T& distance) const;

      virtual std::string internal_members(const std::string& indentation, bool prefix_with_classname = false) const;

      virtual std::string to_string(const std::string& base_indentation,
         const std::string& level_indentation = "  ") const;

      virtual std::string name() const
      {
         return "aggregate";
      }

      virtual intersection_capabilities get_intersection_capabilities() const
      {
         intersection_capabilities caps = intersection_capabilities::ALL;

         // Checkme! Are any of these capabilities disjoint (like the ones in the object capabilities)?
         for( typename shape_list_type::const_iterator iter = shape_list.begin();
              iter != shape_list.end();
              ++iter )
         {
            caps &= (*iter)->get_intersection_capabilities();
         }
         return caps;
      }
      virtual object_capabilities get_object_capabilities() const;

      void add(shape_pointer_type& sp);
      size_t size() const;
      shape_pointer_type& operator[](size_t index);
      const shape_pointer_type& operator[](size_t index) const;
   }; // class aggregate



   //----------------------------------------
   // Default constructor for class aggregate
   //----------------------------------------
   template<class T>
   aggregate<T>::aggregate():
      shape<T>()
   {

   } // aggregate()

   //-------------------------------
   // Destructor for class aggregate
   //-------------------------------
   template<class T>
   aggregate<T>::~aggregate()
   {

   } // ~aggregate()

   //-------------------------------------
   // Copy constructor for class aggregate
   //-------------------------------------
   template<class T>
   aggregate<T>::aggregate(const aggregate<T>& old):
      shape<T>(old),
      shape_list(old.shape_list)
   {
      // Insert any copy-assignment here. DELETEME
   } // aggregate(aggregate)

   //----------------------------------------
   // Assignment operator for class aggregate
   //----------------------------------------
   template<class T>
   aggregate<T>& aggregate<T>::operator= (const aggregate<T>& old)
   {
      // Generic check for self-assignment
      if( &old != this )
      {
         shape_list = old.shape_list;

         shape<T>::operator=(old);
      }
      return(*this);
   } // aggregate::operator=(aggregate)


   template <class T>
   bool aggregate<T>::inside(const point3<T>& p) const
   {
      bool inside_something = false;
      for( typename shape_list_type::const_iterator iter = shape_list.begin();
           ( ( iter != shape_list.end() ) &&
              !inside_something );
           ++iter )
      {
         if( (*iter)->inside(p) )
         {
            inside_something = true;
            break;
         }
      }
      return inside_something;
   }

   template <class T>
   bool aggregate<T>::intersects(const sphere<T>& s) const
   {
      bool intersects_something = false;
      for( typename shape_list_type::const_iterator iter = shape_list.begin();
           ( ( iter != shape_list.end() ) &&
              !intersects_something );
           ++iter )
      {
         if( (*iter)->intersects(s) )
         {
            intersects_something = true;
         }
      }
      return intersects_something;
   }

   template <class T>
   bool aggregate<T>::intersects(const plane<T>& p) const
   {
      bool intersects_something = false;
      for( typename shape_list_type::const_iterator iter = shape_list.begin();
           ( ( iter != shape_list.end() ) &&
              !intersects_something );
           ++iter )
      {
         if( (*iter)->intersects(p) )
         {
            intersects_something = true;
         }
      }
      return intersects_something;
   }

   template <class T>
   bool aggregate<T>::quick_intersection(const unit_line3<T>& line, T time, T& distance) const
   {
      bool hit_something = false;
      T closest = line.limits().end() + 1;

      for( typename shape_list_type::const_iterator iter = shape_list.begin();
           ( iter != shape_list.end() );
           ++iter )
      {
         T dist;
         if( (*iter)->quick_intersection(line, time, dist) )
         {
            if( dist < closest )
            {
               // NOTE: The minimum does not need to be tested, because it must be
               // within the range of the line to be considered a hit.  We'll just
               // save that comparison, as this is slow enough already (for a quick
               // intersection test).
               closest = dist;
               hit_something = true;
            }
         }
      }
      return hit_something;
   }

   template <class T>
   bool aggregate<T>::intersects_line(const line3<T>& line,
      intersection_info<T>& intersection,
      const intersection_requirements& requirements) const
   {
      return shape<T>::intersects_line(line, intersection, requirements);
   }

   template <class T>
   bool aggregate<T>::intersects_line(const unit_line3<T>& line,
      intersection_info<T>& intersection,
      const intersection_requirements& requirements) const
   {
      bool intersects_something = false;

      // Clear it out...
      intersection = intersection_info<T>();

      for( typename shape_list_type::const_iterator iter = shape_list.begin();
           ( iter != shape_list.end() );
           ++iter )
      {
         intersection_info<T> temp_intersection;
         if( (*iter)->intersects_line(line, temp_intersection, requirements) )
         {

            if( intersects_something && requirements.needs_containers() )
            {
               temp_intersection.append_container(this);
            }

            if( ! requirements.needs_all_hits() )
            {
               // This is the easy case, because the entire hit information is
               // replaced if the hit is closer than the current (if any).
               if( !intersects_something )
               {
                  intersects_something = true;
                  intersection = temp_intersection;
               }
               else
               {
                  if( temp_intersection.get_first_distance() < intersection.get_first_distance() )
                  {
                     intersection = temp_intersection;
                  }
                  else
                  {
                     // The existing hit is closer...
                  }
               }
            } // !all hits
            else
            {
               // This is the harder case, or at least messier case, because some of
               // the information will need to be preserved (I've chosen the shape,
               // the distance), for later comparison.

               // Append the complete intersection information, for later retrieval.
               intersection.append_intersection(temp_intersection);
               if( !intersects_something )
               {
                  // Copy some of the values (distance, shape) to the main intersection container.
                  if( temp_intersection.have_shape() )
                  {
                     intersection.set_shape(temp_intersection.get_shape());
                  }
                  if( temp_intersection.have_distance() )
                  {
                     intersection.set_first_distance(temp_intersection.get_first_distance());
                  }
               } // !intersects something
               else // Do a comparison before assigning.
               {
                  if( (temp_intersection.have_distance() && intersection.have_distance()) &&
                     (temp_intersection.get_first_distance() < intersection.get_first_distance()) )
                  {
                     intersection.set_first_distance(temp_intersection.get_first_distance());
                     // Copy some of the values (distance, shape, line) to the main intersection container.
                     if( temp_intersection.have_shape() )
                     {
                        intersection.set_shape(temp_intersection.get_shape());
                     }
                  } // this hit is closer than any preexisting hits.
               } // prior hits
            } // all hits required
         } // if the shape intersects the line.
      } // for each shape.

      return intersects_something;
   }


   template <class T>
   bool aggregate<T>::intersects_ray(const ray_parameters<T>& ray,
      intersection_info<T>& intersection,
      const intersection_requirements& requirements) const
   {
      bool intersects_something = false;

      // Clear it out...
      intersection = intersection_info<T>();

      for( typename shape_list_type::const_iterator iter = shape_list.begin();
           ( iter != shape_list.end() );
           ++iter )
      {
         intersection_info<T> temp_intersection;
         if( (*iter)->intersects_ray(ray, temp_intersection, requirements) )
         {

            if( intersects_something && requirements.needs_containers() )
            {
               temp_intersection.append_container(this);
            }

            if( ! requirements.needs_all_hits() )
            {
               // This is the easy case, because the entire hit information is
               // replaced if the hit is closer than the current (if any).
               if( !intersects_something )
               {
                  intersects_something = true;
                  intersection = temp_intersection;
               }
               else
               {
                  if( temp_intersection.get_first_distance() < intersection.get_first_distance() )
                  {
                     intersection = temp_intersection;
                  }
                  else
                  {
                     // The existing hit is closer...
                  }
               }
            } // !all hits
            else
            {
               // This is the harder case, or at least messier case, because some of
               // the information will need to be preserved (I've chosen the shape,
               // the distance), for later comparison.

               // Append the complete intersection information, for later retrieval.
               intersection.append_intersection(temp_intersection);
               if( !intersects_something )
               {
                  // Copy some of the values (distance, shape) to the main intersection container.
                  if( temp_intersection.have_shape() )
                  {
                     intersection.set_shape(temp_intersection.get_shape());
                  }
                  if( temp_intersection.have_distance() )
                  {
                     intersection.set_first_distance(temp_intersection.get_first_distance());
                  }
               } // !intersects something
               else // Do a comparison before assigning.
               {
                  if( (temp_intersection.have_distance() && intersection.have_distance()) &&
                     (temp_intersection.get_first_distance() < intersection.get_first_distance()) )
                  {
                     intersection.set_first_distance(temp_intersection.get_first_distance());
                     // Copy some of the values (distance, shape, line) to the main intersection container.
                     if( temp_intersection.have_shape() )
                     {
                        intersection.set_shape(temp_intersection.get_shape());
                     }
                  } // this hit is closer than any preexisting hits.
               } // prior hits
            } // all hits required
         } // if the shape intersects the line.
      } // for each shape.

      return intersects_something;
   }


   template <class T>
   std::string aggregate<T>::internal_members(const std::string& indentation, bool prefix_with_classname) const
   {
      std::string retval;
      std::string internal_tagging = indentation;

      if( prefix_with_classname )
      {
         internal_tagging += aggregate<T>::name() + "::";
      }

      retval += indentation + string_format("intersection_capabilities=%1\n", get_intersection_capabilities().to_string());
      retval += indentation + string_format("object_capabilities=%1\n", get_object_capabilities().to_string());
      return retval;
   }

   template <class T>
   std::string aggregate<T>::to_string(const std::string& indent,
      const std::string& level_indent) const
   {
      std::string ret_value = ( indent + "aggregate\n" +
         indent + "{\n"  +
         aggregate<T>::internal_members(indent + level_indent, false) + "\n" );

      std::string next_level = indent + level_indent;

      for( typename shape_list_type::const_iterator iter = shape_list.begin();
           iter != shape_list.end();
           ++iter )
      {
         ret_value += (*iter)->to_string(next_level, level_indent) + "\n";
      }

      ret_value += ( indent + "}" );

      return ret_value;
   }

   template <class T>
   object_capabilities aggregate<T>::get_object_capabilities() const
   {
      object_capabilities caps = object_capabilities::ALL;
      caps &= ~object_capabilities::MOVABLE;
      caps &= ~object_capabilities::SIMPLE;
      caps |= object_capabilities::CONTAINER;
      caps &= ~object_capabilities::IMPLICIT;

      for( typename shape_list_type::const_iterator iter = shape_list.begin();
           iter != shape_list.end();
           ++iter )
      {
         object_capabilities obj_caps = (*iter)->get_object_capabilities();

         if( obj_caps & object_capabilities::INFINITE )
         {
            caps &= ~object_capabilities::BOUNDABLE;
         }
         if( obj_caps & object_capabilities::BOUNDABLE )
         {
            caps &= ~object_capabilities::INFINITE;
         }
         if( obj_caps & object_capabilities::MOVABLE )
         {
            caps |= object_capabilities::MOVABLE;
         }
         if( !(obj_caps & object_capabilities::POLYGONIZATION) )
         {
            caps &= ~object_capabilities::POLYGONIZATION;
         }
         if( obj_caps & object_capabilities::IMPLICIT )
         {
            caps |= object_capabilities::IMPLICIT;
         }
      }
      return caps;
   }

   template <class T>
   void aggregate<T>::add(shape_pointer_type& sp)
   {
      shape_list.push_back(sp);
   }

   template <class T>
   size_t aggregate<T>::size() const
   {
      return shape_list.size();
   }

   template <class T>
   typename aggregate<T>::shape_pointer_type& aggregate<T>::operator[](size_t index)
   {
      return shape_list[index];
   }

   template <class T>
   const typename aggregate<T>::shape_pointer_type& aggregate<T>::operator[](size_t index) const
   {
      return shape_list[index];
   }

} // namespace amethyst


#endif /* !defined(AMETHYST__AGGREGATE_HPP) */
