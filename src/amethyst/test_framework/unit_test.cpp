#include "unit_test.hpp"
#include "unit_test_aggregator.hpp"
#include "unit_test_exceptions.hpp"

namespace amethyst
{
    namespace test
    {
        namespace
        {
            const unit_test* active_test = nullptr;

            struct active_test_scope
            {
                active_test_scope(const unit_test* t) : m_test(active_test)
                {
                    active_test = t;
                }
                ~active_test_scope()
                {
                    active_test = m_test;
                }
                const unit_test* m_test;
            };
        }

        const unit_test* current_test()
        {
            return active_test;
        }

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
            active_test_scope t(this);
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
