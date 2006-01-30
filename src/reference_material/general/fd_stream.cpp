/*
 * $Id: fd_stream.cpp,v 1.1 2006/01/30 11:24:53 kpharris Exp $
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

#include "fd_stream.hpp"
#include <streambuf>
#include <cstdio>
#include <ios>


namespace amethyst
{

	/**
	 *
	 * A private class for handling the streambuf operations.
	 *
	 * @author Kevin Harris <kpharris@users.sourceforge.net>
	 * @version $Revision: 1.1 $
	 *
	 */
	class fd_ostreambuf : public std::streambuf
	{
		friend class fd_ostream;
	private:
		int fd;
	public:
		fd_ostreambuf(int file_descriptor) :
			fd(file_descriptor)
		{
		}

		virtual ~fd_ostreambuf()
		{
		}

		virtual std::streambuf::int_type overflow(std::streambuf::int_type c)
		{
			if( c != EOF )
			{
				if( write(fd, &c, sizeof(char))  == sizeof(char) )
				{
					return 1;
				}
			}
			return EOF;
		}
		std::streamsize xsputn (std::streambuf::char_type* s, std::streamsize n)
		{
			return ::write(fd, s,n);
		}
	};

	//----------------------------------------
	// Default constructor for class fd_ostream
	//----------------------------------------
	fd_ostream::fd_ostream(int file_descriptor):
		std::ostream(0),
		buf(new fd_ostreambuf(file_descriptor))
	{
		std::ostream::rdbuf(buf);
	} // fd_ostream()

	//-------------------------------
	// Destructor for class fd_ostream
	//-------------------------------
	fd_ostream::~fd_ostream()
	{
		delete buf;
	} // ~fd_ostream()


#if 0
	//----------------------------------------
	// Default constructor for class fd_istream
	//----------------------------------------
	fd_istream::fd_istream():
		std::istream()
							  // Class initializers go here. DELETEME
	{

	} // fd_istream()

	//-------------------------------
	// Destructor for class fd_istream
	//-------------------------------
	fd_istream::~fd_istream()
	{

	} // ~fd_istream()

#endif  // 0

} // namespace amethyst

