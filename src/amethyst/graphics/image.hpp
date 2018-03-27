/*
 * $Id: image.hpp,v 1.7 2008/06/21 22:25:10 kpharris Exp $
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

#if       !defined(AMETHYST__IMAGE_HPP)
#define            AMETHYST__IMAGE_HPP

// --------------------------------------
// Default include of parent class header
// --------------------------------------
#include "amethyst/graphics/raster.hpp"
#include "amethyst/graphics/rgbcolor.hpp"

namespace amethyst
{

	// Sorry about these comments, as they are meant to be viewed in html form.
	/**
	 *
	 * A generic class which combines the capabilities of raster and rgbcolor.
	 * This class has almost no functionality of its own.
	 *
	 * Note the neat feature of this class: it can be reinterpreted as a pointer
	 * to the base type given to it.
	 *
	 * For example:<br>
	 * <font color="#008800">image</font>&lt;<font color="#000088">unsigned
	 * char</font>&gt im(640, 480);
	 *
	 * <font color="#880000">
	 * // This next variable can be treated as a single array of size      <br>
	 * // im.numpixels( ) in length, which would be 307200 in this case.   <br>
	 * </font>
	 * <font color="#000088">unsigned char</font>*
	 * image_data</font> =
	 * im.<font color="#008800">reinterpret</font>
	 * &lt;<font color="#000088">unsigned char</font>*&gt; ( );<br>
	 * ...<br>
	 * <font color="#880000">
	 * // Act on the data, being careful not to go beyond bounds on either <br>
	 * // end, or (whether intentional or not) deallocate the pointer.     <br>
	 * </font>
	 *
	 * @author Kevin Harris <kpharris@users.sourceforge.net>
	 * @version $Revision: 1.7 $
	 * @see raster
	 * @see rgbcolor
	 *
	 */
	template <typename T>
	using image = raster<rgbcolor<T>>;
}


#endif /* !defined(AMETHYST__IMAGE_HPP) */

