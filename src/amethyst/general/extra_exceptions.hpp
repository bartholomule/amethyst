/*
 * $Id: extra_exceptions.hpp,v 1.1 2008/06/16 10:17:47 kpharris Exp $
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

#if       !defined(AMETHYST__EXTRA_EXCEPTIONS_HPP)
#define            AMETHYST__EXTRA_EXCEPTIONS_HPP

#include <stdexcept>

namespace amethyst
{

    /**
     *
     * Thrown to indicate a size error (for assignment of arrays, etc).
     *
     * @author Kevin Harris <kpharris@users.sourceforge.net>
     * @version $Revision: 1.1 $
     *
     */
    class size_mismatch : public std::runtime_error
    {
    public:
        /** The constructor that is used when it is thrown with a string. */
        explicit size_mismatch(const std::string& _arg);
    }; // class size_mismatch

    /*
       Import the out_of_range class into this namespace.
     */
    using std::out_of_range;

} // namespace amethyst


#endif /* !defined(AMETHYST__EXTRA_EXCEPTIONS_HPP) */

