/*
 * $Id: defines.hpp,v 1.4 2004/06/01 03:50:25 kpharris Exp $
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

#if       !defined(AMETHYST__DEFINES_HPP)
#define            AMETHYST__DEFINES_HPP

#define AMETHYST_EPSILON (1e-9)
#define AMETHYST_EPS_SRQ (AMETHYST_EPSILON * AMETHYST_EPSILON)

#include <limits.h>
#include <limits>

// This may need to be much lower for everything (huge number)
#define AMETHYST_DEPTH_MAX (std::numeric_limits<long>::max())


namespace amethyst
{
  // Put anything local here.  
  
} // namespace amethyst


#endif /* !defined(AMETHYST__DEFINES_HPP) */

