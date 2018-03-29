/*
 * $Id: fd_stream.hpp,v 1.1 2006/01/30 11:24:53 kpharris Exp $
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

#if       !defined(AMETHYST__FD_STREAM_HPP)
#define            AMETHYST__FD_STREAM_HPP

#include "platform.hpp"
#include <istream>
#include <ostream>
#include <iostream>

namespace amethyst
{
    class fd_ostreambuf;

#if defined(WINDOWS)
    using file_handle_type = HANDLE;
#else
    using file_handle_type = int;
#endif


    // 0 = stdin, 1 = stdout, 2 = stderr
    file_handle_type get_std_handle(int unix_number);

    /**
     *
     * A simple class for creating an output stream from a file descriptor.
     *
     * @author Kevin Harris <kpharris@users.sourceforge.net>
     * @version $Revision: 1.1 $
     *
     */
    class fd_ostream : public std::ostream
    {

    public:
        explicit fd_ostream(file_handle_type file_descriptor = get_std_handle(1));
        fd_ostream(const fd_ostream& old) = delete;
        fd_ostream(fd_ostream&& old);
        virtual ~fd_ostream();
        fd_ostream& operator=(const fd_ostream& old) = delete;
        fd_ostream& operator=(fd_ostream&& old);
    protected:
        fd_ostreambuf* buf;
    }; // class fd_ostream

#if 0
    class fd_istreambuf;

    /**
     *
     * A simple class for creating an input stream from a file descriptor.
     *
     * @author Kevin Harris <kpharris@users.sourceforge.net>
     * @version $Revision: 1.1 $
     *
     */
    class fd_istream : public std::istream
    {

    protected:
        fd_istreambuf* buf;

    public:
        /** Default constructor */
        explicit fd_istream(int file_descriptor = 0);

        /** Destructor */
        virtual ~fd_istream();

    private:
        /** Copy constructor */
        fd_istream(const fd_istream& old); // not implemented.

        /** Assignment operator */
        fd_istream& operator= (const fd_istream& old); // not implemented

    }; // class fd_istream

#endif // 0

} // namespace amethyst


#endif /* !defined(AMETHYST__FD_STREAM_HPP) */

