/*
 * $Id: aggregate.hpp,v 1.4 2004/03/27 19:33:28 kpharris Exp $
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
#include "shape.hpp"

#include <vector>
#include <rc_pointer.hpp>

namespace amethyst
{ 

  /**
   * 
   * An aggregate (collection) of shapes.
   * 
   * @author Kevin Harris <kpharris@users.sourceforge.net>
   * @version $Revision: 1.4 $
   * 
   */
  template<class T>
  class aggregate : public shape<T>
  {
  public:
    typedef rc_pointer<shape<T> > shape_pointer_type;
    typedef std::vector<shape_pointer_type> shape_list_type;
    
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
				 intersection_info<T>& intersection) const;

    virtual std::string internal_members(const std::string& indentation, bool prefix_with_classname = false) const;
    
    virtual std::string to_string(const std::string& base_indentation,
				  const std::string& level_indentation = "  ") const;

    virtual std::string name() const { return "aggregate"; }    

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
    if( &old != this)
    {
      shape_list = old.shape_list;

      shape<T>::operator=(old);
    }
    return (*this);
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
  bool aggregate<T>::intersects_line(const unit_line3<T>& line,
				     intersection_info<T>& intersection) const
  {
    bool intersects_something = false;

    for( typename shape_list_type::const_iterator iter = shape_list.begin();
	 ( iter != shape_list.end() );
	 ++iter )
    {
      intersection_info<T> temp_intersection;
      if( (*iter)->intersects_line(line, temp_intersection) )
      {
	if( !intersects_something )
	{
	  intersects_something = true;
	  intersection = temp_intersection;
	}
	else
	{
	  if( temp_intersection.get_distance() < intersection.get_distance() )
	  {
	    intersection = temp_intersection;
	  }
	  else
	  {
	    // The existing hit is closer...
	  }
	}
      }
    }
    return intersects_something;
  }
  
  template <class T>
  std::string aggregate<T>::internal_members(const std::string& indentation, bool prefix_with_classname) const
  {
    std::string retval;

    return retval;
  }

  template <class T>
  std::string aggregate<T>::to_string(const std::string& indent,
				      const std::string& level_indent) const
  {
    std::string ret_value = ( indent + "aggregate\n" +
			      indent + "{\n" );

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

