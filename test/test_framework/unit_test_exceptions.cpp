/*
 * $Id: unit_test_exceptions.cpp,v 1.1 2008/06/09 16:31:18 kpharris Exp $
 *
 * Part of "Amethyst Palace" - A playground for future graphics ideas
 * Copyright (C) 2007 Kevin Harris
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

#include <string>
#include "unit_test_exceptions.hpp"
#include "unit_test_info.hpp"

namespace amethyst
{
	namespace test
	{
		test_exception::test_exception(const std::string& why, const std::string& test_name, const std::string& file, int line)
			: reason(why)
			, info(test_name, file, line)
		{
		}
		test_exception:: test_exception(const std::string& why, const test_information& test_info)
			: reason(why)
			, info(test_info)
		{
		}

		AMETHYST_DEFINE_EXCEPTION(test_aborted);
		AMETHYST_DEFINE_EXCEPTION(never_thrown); // Never thrown, used as a dummy exception
	}
}
