/*
 * $Id: unit_test.cpp,v 1.1 2008/06/09 16:31:18 kpharris Exp $
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
#include "unit_test_exceptions.hpp"

namespace amethyst
{
    namespace test
    {
        unit_test::unit_test(const std::string& name, test_results* chain_results)
            : result_counter(new test_results(name, chain_results))
            , test_name(name)
        {
        }
        unit_test::~unit_test()
        {
            delete result_counter;
        }

        void unit_test::run_test(const test_information& info) const
        {
            try
            {
                do_run_test(info);
            }
            catch (const test_aborted_exception& e)
            {
                result_counter->test_failed(e.info, "Aborted: " + e.reason);
                throw;
            }
            catch (const test_exception& e)
            {
                result_counter->test_failed(info, string_format("Test aborted with a test exception: %1:%2: %3", e.info.filename, e.info.line_number, e.reason));
                throw;
            }
            catch (...)
            {
                result_counter->test_failed(info, "Aborted with unknown exception");
                throw;
            }
        }

        void unit_test::run_test(const std::string& file, int lineno) const
        {
            test_information info(test_name, file, lineno);
            run_test(info);
        }

        void unit_test::run_test() const
        {
            run_test("unknown", -1);
        }

        void unit_test::test_started(const test_information& info) const
        {
            result_counter->test_started(info);
        }
        void unit_test::test_passed(const test_information& info) const
        {
            result_counter->test_passed(info);
        }
        void unit_test::test_failed(const test_information& info, const std::string& reason) const
        {
            result_counter->test_failed(info, reason);
        }
        void unit_test::print_results() const
        {
            result_counter->print_results();
        }
    }
}
