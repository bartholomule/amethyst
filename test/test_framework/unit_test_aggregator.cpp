/*
 * $Id: unit_test_aggregator.cpp,v 1.1 2008/06/09 16:31:18 kpharris Exp $
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

#include "unit_test.hpp"
#include "unit_test_aggregator.hpp"
#include "unit_test_info.hpp"
#include "general/string_format.hpp"

namespace amethyst
{
	namespace test
	{
		// This is used outside this file...
		test_results global_test_results("global test results", NULL);

		test_results::test_results(const std::string& name, test_results* outer_test_results)
			: count_failed(0)
			, count_passed(0)
			, count_tried(0)
			, results_name(name)
			, chained_results(outer_test_results)
			, printed(false)
		{
		}

		test_results::~test_results()
		{
			if( !printed )
			{
				print_results();
			}
		}

		void test_results::print_results(std::ostream& o) const
		{
			printed = true;

			size_t count_unfinished = count_tried - (count_passed + count_failed);
			size_t percent_passed = 100;
			size_t percent_failed = 0;
			size_t percent_unfinished = 0;
			if( count_tried > 0 )
			{
				percent_passed = (100 * count_passed) / count_tried;
				percent_failed = (100 * count_failed) / count_tried;
				percent_unfinished = (100 * count_unfinished) / count_tried;
			}
			o << "\n";
			o << string_format("Test results for \"%1\":\n", results_name);
			o << "---------------------------\n";
			o << string_format("  Tests passed: %1 of %2 (%3%%)\n", count_passed, count_tried, percent_passed);
			o << string_format("  Tests failed: %1 of %2 (%3%%)\n", count_failed, count_tried, percent_failed);
			o << string_format("  Tests unfinished: %1 of %2 (%3%%)\n", count_unfinished, count_tried, percent_unfinished);
			o << "---------------------------\n";
			o << std::flush;
		}

		void test_results::notify(const test_information& info, const std::string& text) const
		{
			std::cerr << string_format("%1:%2:%3", info.filename, info.line_number, text) << std::endl;
		}
		void test_results::test_started(const test_information& info, bool chained)
		{
			if( !chained )
			{
				notify(info, string_format("info: Started test %1", info.test_name));
			}
			++count_tried;
			if( chained_results )
			{
				chained_results->test_started(info, true);
			}
		}
		void test_results::test_passed(const test_information& info, bool chained)
		{
			if( !chained )
			{
				notify(info, "info: Test passed\n"); // added vertical whitespace to assist emacs in finding error lines
			}
			++count_passed;
			if( chained_results )
			{
				chained_results->test_passed(info, true);
			}
		}
		void test_results::test_failed(const test_information& info, const std::string& reason, bool chained)
		{
			if( !chained )
			{
				notify(info, string_format("error: %1\n", reason));  // added vertical whitespace to assist emacs in finding error lines
			}
			++count_failed;
			if( chained_results )
			{
				chained_results->test_failed(info, reason, true);
			}
		}

		bool test_results::something_failed() const
		{
			return count_failed > 0;
		}
	}
}
