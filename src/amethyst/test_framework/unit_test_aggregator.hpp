/*
 * $Id: unit_test_aggregator.hpp,v 1.1 2008/06/09 16:31:18 kpharris Exp $
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

#if       !defined(UNIT_TEST_AGGREGATOR_HPP_INCLUDED)
#define            UNIT_TEST_AGGREGATOR_HPP_INCLUDED

#include <iostream>
#include <string>

#include "unit_test_fwd.hpp"

namespace amethyst
{
    namespace test
    {
        class test_results
        {
        public:
            test_results(const std::string& name, test_results* outer_test_results = & global_test_results);
            virtual ~test_results();
            void print_results(std::ostream& o = std::cerr) const;

            void notify(const test_information& info, const std::string& text) const;
            void test_started(const test_information& info, bool chained = false);
            void test_passed(const test_information& info, bool chained = false);
            void test_failed(const test_information& info, const std::string& reason, bool chained = false);

            bool something_failed() const;

        private:
            size_t count_failed;
            size_t count_passed;
            size_t count_tried;
            std::string results_name;
            test_results* chained_results;
            mutable bool printed; // Not really causing the structure to change, just suppressing output in the destructor.
        };
    }
}

#endif /* !defined(UNIT_TEST_AGGREGATOR_HPP_INCLUDED) */
