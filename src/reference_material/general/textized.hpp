/*
 * $Id: textized.hpp,v 1.1 2008/06/17 16:11:11 kpharris Exp $
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

#include "amethyst/config.hpp"

#if defined(HAVE_LIBINTL_H)
#include <libintl.h>
#define intl(x) gettext (x)
#else
#define intl(x) x
#endif

#endif /* !defined(AMETHYST__TEXTIZED_HPP) */
