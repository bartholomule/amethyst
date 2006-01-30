/*
 * $Id: stream_reference.hpp,v 1.1 2006/01/30 11:24:53 kpharris Exp $
 *
 * Part of "Amethyst" -- A playground for graphics development.
 * Copyright (C) 2006 Kevin Harris
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

#if       !defined(AMETHYST__STREAM_REFERENCE_HPP)
#define            AMETHYST__STREAM_REFERENCE_HPP

#include <ostream>
#include <istream>
#include <iostream>
#include "rc_pointer.hpp"

namespace amethyst
{
	typedef rc_pointer<std::ostream> output_stream_ref;
	extern output_stream_ref stream_stdout;
	extern output_stream_ref stream_stderr;

	// Anything that follows is not complete, and as such cannot be used.
	typedef rc_pointer<std::istream> input_stream_ref;
	extern input_stream_ref stream_stdin;

	typedef rc_pointer<std::iostream> io_stream_ref;
} // namespace amethyst


#endif /* !defined(AMETHYST__STREAM_REFERENCE_HPP) */

