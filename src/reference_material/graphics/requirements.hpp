/*
 * $Id: requirements.hpp,v 1.1 2004/05/17 07:17:04 kpharris Exp $
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

#if       !defined(AMETHYST__REQUIREMENTS_HPP)
#define            AMETHYST__REQUIREMENTS_HPP

#include "capabilities.hpp"

namespace amethyst
{ 
  /**
   * 
   * Requirements for an intersection.
   * 
   * @author Kevin Harris <kpharris@users.sourceforge.net>
   * @version $Revision: 1.1 $
   * 
   */
  class intersection_requirements
  {

  public:
    /* Default constructor.  Sets default values */
    intersection_requirements();

    /** Destructor */
    virtual ~intersection_requirements();

    /* Uses default copy constructor, assignment operator, etc */

    // Specifies if only the first hit is required, so that large aggregates
    // need only find the first hit, and not search for the closest.     
    bool needs_first_only() const
    {
      return my_requirements & intersection_capabilities::HIT_FIRST;
    }

    // Specifies if all hit points on the object are required, if set to false,
    // only the closest is required.     
    bool needs_all_hits() const
    {
      return my_requirements & intersection_capabilities::HIT_ALL;
    }
    // Specifies if the normal at the point of intersection is required.
    bool needs_normal() const
    {
      return my_requirements & intersection_capabilities::NORMAL_CALCULATION;
    }
    // Specifies if all object pointers, including that of the containing
    // objects are required.    
    bool needs_containers() const
    {
      return my_requirements & intersection_capabilities::HIT_CONTAINERS;
    }
    // Specifies if a uv coordinate on the surface is required.
    bool needs_uv() const
    {
      return my_requirements & intersection_capabilities::UV_CALCULATION;
    }
    // Specifies if a local coordinate system (at the point of the hit) is
    // required.    
    bool needs_local_coord_system() const
    {
      return my_requirements & intersection_capabilities::LOCAL_SYSTEM_CALCULATION;
    }

    void force_first_only(bool val)
    {
      if( val )
      {
        my_requirements |= intersection_capabilities::HIT_FIRST;
        my_requirements &= ~intersection_capabilities::HIT_ALL;
      }
      else
      {
        my_requirements &= ~intersection_capabilities::HIT_FIRST;
        my_requirements |= intersection_capabilities::HIT_ALL;
      }
    }
    void force_all_hits(bool val)
    {
      if( val )
      {
        my_requirements &= ~intersection_capabilities::HIT_FIRST;
        my_requirements |= intersection_capabilities::HIT_ALL;
      }
      else
      {
        my_requirements |= intersection_capabilities::HIT_FIRST;
        my_requirements &= ~intersection_capabilities::HIT_ALL;
      }
    }
    void force_normal(bool val)
    {
      if( val )
      {
        my_requirements |= intersection_capabilities::NORMAL_CALCULATION;
      }
      else
      {
        my_requirements &= ~intersection_capabilities::NORMAL_CALCULATION;      
      }
    }
    void force_containers(bool val)
    {
      if( val )
      {
        my_requirements |= intersection_capabilities::HIT_CONTAINERS;
      }
      else
      {
        my_requirements &= ~intersection_capabilities::HIT_CONTAINERS;
      }
    }
    void force_uv(bool val)
    {
      if( val )
      {
        my_requirements |= intersection_capabilities::UV_CALCULATION;
      }
      else
      {
        my_requirements &= ~intersection_capabilities::UV_CALCULATION;
      }
    }
    void force_local_coord_system(bool val)
    {
      if( val )
      {
        my_requirements |= intersection_capabilities::LOCAL_SYSTEM_CALCULATION;
      }
      else
      {
        my_requirements &= ~intersection_capabilities::LOCAL_SYSTEM_CALCULATION;
      }      
    }

    intersection_capabilities get_matching_capabilities() const;
    bool capabilities_match(intersection_capabilities c) const;

  protected:
    intersection_capabilities my_requirements;
  }; // class intersection_requirements

} // namespace amethyst

#endif /* !defined(AMETHYST__REQUIREMENTS_HPP) */

