/*
 * $Id: unit_test_auto.cpp,v 1.1 2008/06/09 16:31:18 kpharris Exp $
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

#include "unit_test_auto.hpp"
#include "unit_test_aggregator.hpp"
#include <iostream>

namespace amethyst
{
    namespace test
    {
        namespace // anonymous
        {
            struct automatic_test_list
            {
                unit_test* test;
                test_information* info;
                automatic_test_list* next;
            };
            automatic_test_list auto_test_list = { NULL, NULL, NULL };

            void delete_auto_tests()
            {
                automatic_test_list* foo = auto_test_list.next;
                while (foo)
                {
                    automatic_test_list* bar = foo;
                    foo = foo->next;
                    delete bar->info;
                    delete bar;
                }
            }

            bool run_auto_tests()
            {
                for (automatic_test_list* foo = auto_test_list.next; foo; foo = foo->next)
                {
                    if (foo->test)
                    {
                        if (foo->info)
                        {
                            foo->test->run_test(*foo->info);
                        }
                        else
                        {
                            foo->test->run_test("auto_unit_test", -1);
                        }
                        foo->test->print_results();
                    }
                }
                return !global_test_results.something_failed();
            }
        } // end anonymous namespace

        void add_auto_test(unit_test* test, const test_information& info)
        {
            // FIXME! exception safety
            // Not exception safe, but we really shouldn't run out of memory.
            automatic_test_list* tl = new automatic_test_list();
            tl->test = test;
            tl->info = new test_information(info);
            tl->next = NULL;

            // Append the new test to the list...
            automatic_test_list* foo = &auto_test_list;
            for (; foo->next; foo = foo->next)
            {
            }
            foo->next = tl;
        }

        int unit_test_auto_main_impl(int argc, const char** argv)
        {
            int return_value = 10;
            std::cerr << "Starting auto unit test main" << std::endl;
            try
            {
                bool b = run_auto_tests();

                std::cerr << "Done executing auto unit tests." << std::endl;

                global_test_results.print_results();

                if (b)
                {
                    std::cerr << "All tests passed" << std::endl;
                    return_value = 0;
                }
                else
                {
                    std::cerr << "Not everything passed." << std::endl;
                    return_value = 1;
                }
            }
            catch (const test_exception& e)
            {
                std::cerr << string_format("ERROR: Exception escaped (to main) from %1:%2: %3", e.info.filename, e.info.line_number, e.reason) << std::endl;
                return_value = 2;
            }
            catch (...)
            {
                std::cerr << "ERROR: Auto test main: Exception escaped tests." << std::endl;
                return_value = 3;
            }
            delete_auto_tests();
            return return_value;
        }
    }
}
