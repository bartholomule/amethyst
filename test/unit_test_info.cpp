/*
 * $Id: unit_test_info.cpp,v 1.1 2007/07/05 21:33:30 kpharris Exp $
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

#include "unit_test_info.hpp"

namespace amethyst
{
	namespace test
	{
		test_information::test_information()
			: test_name("unknown test")
			, filename("unkown file")
			, line_number(-1)
		{
		}
		test_information::test_information(const std::string& name, const std::string& file, int line)
			: test_name(name)
			, filename(file)
			, line_number(line)
		{
		}
	}
}
