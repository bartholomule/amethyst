/*
 * $Id: unit_test_auto.hpp,v 1.1 2007/07/05 21:33:30 kpharris Exp $
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

#if       !defined(UNIT_TEST_AUTO_HPP_INCLUDED)
#define            UNIT_TEST_AUTO_HPP_INCLUDED

#include "unit_test.hpp"
#include "unit_test_info.hpp"
#include "unit_test_exceptions.hpp" // Not strictly needed but used to reduce required headers.

namespace amethyst
{
	namespace test
	{
		void add_auto_test(unit_test* test, const test_information& info);

#define AUTO_UNIT_TEST(name) \
		class auto_test_##name : public ::amethyst::test::unit_test \
		{ \
		public: \
			auto_test_##name() : unit_test(#name, &::amethyst::test::global_test_results) \
			{ \
				::amethyst::test::add_auto_test(this, TEST_INFORMATION(#name)); \
			} \
			~auto_test_##name() { } \
		private: \
			virtual void do_run_test(const ::amethyst::test::test_information& test_info_data) const; \
		}; \
		auto_test_##name auto_test_creation_##name; \
		void auto_test_##name::do_run_test(const ::amethyst::test::test_information& test_info_data) const


		int unit_test_auto_main_impl(int argc, const char** argv);
	}
}

//  Define this to main if you want it to be called as main...
int auto_unit_test_main(int argc, const char** argv)
{
	return amethyst::test::unit_test_auto_main_impl(argc, argv);
}
#endif /* !defined(UNIT_TEST_AUTO_HPP_INCLUDED) */
