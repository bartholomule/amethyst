/*
 * $Id: stream_reference.cpp,v 1.1 2006/01/30 11:24:53 kpharris Exp $
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

#include "stream_reference.hpp"
#include "fd_stream.hpp"

namespace amethyst
{
#if defined(WINDOWS)
    output_stream_ref stream_stdout(new fd_ostream(GetStdHandle(STD_OUTPUT_HANDLE)));
    output_stream_ref stream_stderr(new fd_ostream(GetStdHandle(STD_ERROR_HANDLE)));

#else
    output_stream_ref stream_stdout(new fd_ostream(1));
    output_stream_ref stream_stderr(new fd_ostream(2));
#endif
} // namespace amethyst

