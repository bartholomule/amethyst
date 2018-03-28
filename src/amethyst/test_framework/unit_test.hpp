/*
 * $Id: unit_test.hpp,v 1.2 2008/06/10 06:57:52 kpharris Exp $
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

#if       !defined(UNIT_TEST_HPP_INCLUDED)
#define            UNIT_TEST_HPP_INCLUDED

#include "unit_test_fwd.hpp"
#include "unit_test_exceptions.hpp"
#include "general/string_format.hpp"
#include <string>
#include <iosfwd>

namespace amethyst
{
	namespace test
	{
		class unit_test
		{
		public:
			unit_test(const std::string& name, test_results* chain_results = &global_test_results);
			virtual ~unit_test();
			void run_test() const;
			void run_test(const test_information& info) const;
			void run_test(const std::string& file, int lineno) const;

			void test_started(const test_information& info) const;
			void test_passed(const test_information& info) const;
			void test_failed(const test_information& info, const std::string& reason) const;

			void print_results() const;
		protected:
			test_results* result_counter;
			std::string test_name;
		private:
			virtual void do_run_test(const test_information& info) const = 0;
		};
	}
}

#define TEST_BOOLEAN(body) \
do { \
	::amethyst::test::test_information local_info = TEST_INFORMATION(::amethyst::string_format("%1_boolean_%2", test_name, __LINE__)); \
	unit_test::test_started(local_info); \
	if( body ) \
	{ \
		unit_test::test_passed(local_info); \
	} \
	else \
	{ \
		unit_test::test_failed(local_info, ::amethyst::string_format("Boolean test failed for \"%1\": (%2)", #body, (body))); \
	} \
} while(0)

#define TEST_COMPARE_GENERIC(a, b, operation) \
do { \
	::amethyst::test::test_information local_info = TEST_INFORMATION(::amethyst::string_format("%1_comparison_%2(%3)", test_name, __LINE__,#operation)); \
	unit_test::test_started(local_info); \
	if( (a) operation (b) ) \
	{ \
		unit_test::test_passed(local_info); \
	} \
	else \
	{ \
		unit_test::test_failed(local_info, ::amethyst::string_format("comparison failed for \"%1 %2 %3\": (%4) %5 (%6)", #a, #operation, #b, a, #operation, b)); \
	} \
} while(0)

#define TEST_COMPARE_EQUAL(a, b) TEST_COMPARE_GENERIC(a,b,==)
#define TEST_COMPARE_NOT_EQUAL(a, b) TEST_COMPARE_GENERIC(a,b,!=)
#define TEST_COMPARE_GREATER(a, b) TEST_COMPARE_GENERIC(a,b,>)
#define TEST_COMPARE_GREATER_EQ(a, b) TEST_COMPARE_GENERIC(a,b,>=)
#define TEST_COMPARE_LESS(a, b) TEST_COMPARE_GENERIC(a,b,<)
#define TEST_COMPARE_LESS_EQ(a, b) TEST_COMPARE_GENERIC(a,b,<=)

#define TEST_COMPARE_CLOSE(a,b,epsilon) \
do { \
	::amethyst::test::test_information local_info = TEST_INFORMATION(::amethyst::string_format("%1_compare_%2", test_name, __LINE__)); \
	unit_test::test_started(local_info); \
	if( ( (((a) - (b)) < (epsilon)) && (((a) - (b)) > (-(epsilon))) )	\
		|| ( (((b) - (a)) < (epsilon)) && (((b) - (a)) > (-(epsilon))) ) ) \
	{ \
		unit_test::test_passed(local_info); \
	} \
	else \
	{ \
		unit_test::test_failed(local_info, ::amethyst::string_format("Closeness test failed for \"%1\" and \"%2\": %3 != %4 (epsilon %5)", #a, #b, (a), (b), (epsilon))); \
	} \
} while(0)

#define TEST_EXCEPTION_GENERIC_TRYBLOCK(tryblock, catchtype, catchblock, catchallblock) \
do { \
	::amethyst::test::test_information local_info = TEST_INFORMATION(::amethyst::string_format("%1_exception_%2", test_name, __LINE__)); \
	unit_test::test_started(local_info); \
	try tryblock \
	catch( const ::amethyst::test::test_aborted_exception& e ) \
	{ \
		unit_test::test_failed(e.info, ::amethyst::string_format("(noticed on %1:%2) Aborted: %3", __FILE__, __LINE__, e.reason)); \
		throw; \
	} \
	catch(const catchtype& ex) catchblock \
	catch(...) catchallblock \
} while(0)

#define TEST_EXCEPTION_THROW(exception_test_body) \
TEST_EXCEPTION_GENERIC_TRYBLOCK( \
	{ exception_test_body; unit_test::test_failed(local_info, ::amethyst::string_format("Test did not throw for \"%1\"", #exception_test_body)); }, \
	::amethyst::test::never_thrown_exception, \
	{ (void) ex; }, \
	{ unit_test::test_passed(local_info); } \
)

#define TEST_EXCEPTION_THROW_SPECIFIC(exception_test_body, exception_type) \
TEST_EXCEPTION_GENERIC_TRYBLOCK( \
	{ exception_test_body; unit_test::test_failed(local_info, ::amethyst::string_format("Test did not throw for \"%1\"", #exception_test_body)); }, \
	exception_type, \
	{ (void) ex; unit_test::test_passed(local_info); }, \
	{ unit_test::test_failed(local_info, ::amethyst::string_format("Caught unexpected exception (not %1) in \"%2\"", #exception_type, #exception_test_body)); } \
)

#define TEST_EXCEPTION_NOTHROW(exception_test_body) \
TEST_EXCEPTION_GENERIC_TRYBLOCK( \
	{ exception_test_body; unit_test::test_passed(local_info); }, \
	::amethyst::test::never_thrown_exception, \
	{ unit_test::test_failed(local_info, ::amethyst::string_format("Test threw exception in \"%1\": %2", #exception_test_body, amethyst::test::what(ex))); }, \
	{ unit_test::test_failed(local_info, ::amethyst::string_format("Test threw exception in \"%1\"", #exception_test_body)); } \
)

#define TEST_EXCEPTION_NOTHROW_SPECIFIC(exception_test_body, exception_type) \
TEST_EXCEPTION_GENERIC_TRYBLOCK( \
	{ exception_test_body; unit_test::test_passed(local_info); }, \
	exception_type, \
	{ unit_test::test_failed(local_info, ::amethyst::string_format("Test threw the disallowed exception (%1) in \"%2\": %3", #exception_type, #exception_test_body, amethyst::test::what(ex))); }, \
	{ unit_test::test_passed(local_info); } \
)

#endif /* !defined(UNIT_TEST_HPP_INCLUDED) */
