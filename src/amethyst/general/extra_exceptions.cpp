/*
 * $Id: extra_exceptions.cpp,v 1.1 2008/06/16 10:17:47 kpharris Exp $
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

#include "extra_exceptions.hpp"


namespace amethyst
{ 

  //-----------------------------------------------
  // Default constructor for class size_mismatch
  //-----------------------------------------------
  size_mismatch::size_mismatch(const std::string& _arg):
    std::runtime_error(_arg)
  {
  } // size_mismatch()


} // namespace amethyst

