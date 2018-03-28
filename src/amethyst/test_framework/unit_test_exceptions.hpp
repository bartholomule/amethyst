/*
 * $Id: unit_test_exceptions.hpp,v 1.1 2008/06/09 16:31:18 kpharris Exp $
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

#if       !defined(UNIT_TEST_EXCEPTIONS_HPP_INCLUDED)
#define            UNIT_TEST_EXCEPTIONS_HPP_INCLUDED

#include <string>
#include "unit_test_info.hpp"

namespace amethyst
{
	namespace test
	{
		struct test_exception
		{
			test_exception(const std::string& why, const std::string& test_name = "unknown test", const std::string& file = "unknown file", int line = -1);
			test_exception(const std::string& why, const test_information& tes_info);
			std::string reason;
			test_information info;
		};

        std::string what(const test_exception& e);
        std::string what(const std::exception& e);

        template<typename T>
        std::string what(const T& t) { return ""; }

		// Declare a function 
#define AMETHYST_DECLARE_EXCEPTION_HELPER(full_name) \
		struct full_name : public test_exception \
		{ \
			full_name(const std::string& why, const std::string& test_name = "unknown test", const std::string& file = "unknown file", int line = -1); \
			full_name(const std::string& why, const test_information& info); \
		};
#define AMETHYST_DECLARE_EXCEPTION(name) AMETHYST_DECLARE_EXCEPTION_HELPER(name##_exception)

#define AMETHYST_DEFINE_EXCEPTION_HELPER(full_name) \
		full_name::full_name(const std::string& why, const std::string& test_name, const std::string& file, int line) \
			: test_exception(why, test_name, file, line) \
		{ \
		} \
		full_name::full_name(const std::string& why, const test_information& ex_info) \
			: test_exception(why, info) \
		{ \
		}
#define AMETHYST_DEFINE_EXCEPTION(name) AMETHYST_DEFINE_EXCEPTION_HELPER(name##_exception)

		AMETHYST_DECLARE_EXCEPTION(test_aborted);
		AMETHYST_DECLARE_EXCEPTION(never_thrown); // Never thrown, used as a dummy exception


#define AMETHYST_THROW(name, reason) throw name##_exception(reason, test_name, __FILE__, __LINE__)
#define AMETHYST_ABORT_TESTS(reason) AMETHYST_THROW(::amethyst::test::test_aborted, reason)
	}
}

#endif /* !defined(UNIT_TEST_EXCEPTIONS_HPP_INCLUDED) */
