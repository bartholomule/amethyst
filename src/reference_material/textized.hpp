/*
 * $Id: textized.hpp,v 1.3 2004/02/14 06:55:17 kpharris Exp $
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

#if       !defined(AMETHYST__TEXTIZED_HPP)
#define            AMETHYST__TEXTIZED_HPP

// Define this value to effectively remove internationalization
#if !defined(ENABLE_NLS) || !ENABLE_NLS

#define gettext(String) (String)
#define gettextN(String) String
#define intl(String) gettext(String)
#define intlN(String) gettextN(String)
#define textdomain(Domain)
#define bindtextdomain(Package, Directory)

#else

#include <libintl.h>
#define intl(String) gettext (String)
#define gettext_noop(String) String
#define intlN(String) gettext_noop (String)


#endif /* defined(ENABLE_NLS) */

#endif /* !defined(AMETHYST__TEXTIZED_HPP) */
