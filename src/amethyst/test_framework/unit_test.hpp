#pragma once

#include "unit_test_fwd.hpp"
#include "unit_test_exceptions.hpp"
#include "general/string_format.hpp"
#include "general/defines.hpp"
#include <string>
#include <iosfwd>

namespace amethyst
{
    namespace test
    {
        class unit_test
        {
        public:
            unit_test(const std::string& name, test_results* chain_results = & global_test_results);
            virtual ~unit_test();
            void run_test() const;
            void run_test(const test_information& info) const;
            void run_test(const std::string& file, int lineno) const;

            void test_started(const test_information& info) const;
            void test_passed(const test_information& info) const;
            void test_failed(const test_information& info, const std::string& reason) const;

            void print_results() const;
            std::string test_name;
        protected:
            test_results* result_counter;
        private:
            virtual void do_run_test(const test_information& info) const = 0;
        };

        const unit_test* current_test();
    }
}

#define TEST_BODY_HELPER(name, body) \
    do { \
        ::amethyst::test::test_information local_info = TEST_INFORMATION(::amethyst::test::current_test()->test_name + "_" + name + "_" + std::to_string(__LINE__)); \
        ::amethyst::test::current_test()->test_started(local_info); \
        if (body) \
        { \
            ::amethyst::test::current_test()->test_passed(local_info); \
        } \
        else \
        { \
            ::amethyst::test::current_test()->test_failed(local_info, name + ::amethyst::string_format(" test failed for \"%1\": (%2)", #body, (body))); \
        } \
    } while (0)


#define TEST_BOOLEAN(body) TEST_BODY_HELPER("boolean", body)

#define TEST_COMPARE_GENERIC(a, b, op, invop) \
    do { \
        ::amethyst::test::test_information local_info = TEST_INFORMATION("compare(" #op  ")_" + std::to_string(__LINE__)); \
        ::amethyst::test::current_test()->test_started(local_info); \
        const auto& aaaa = (a); \
        const auto& bbbb = (b); \
        if (aaaa op bbbb) \
        { \
            ::amethyst::test::current_test()->test_passed(local_info); \
        } \
        else \
        { \
            ::amethyst::test::current_test()->test_failed(local_info, ::amethyst::string_format(" test failed for " #a #op #b ": (%1 " #invop " %2)", aaaa, bbbb)); \
        } \
    } while (0)

// #define TEST_COMPARE_GENERIC(a, b, operation) TEST_BODY_HELPER("compare(" #operation ")", (a) operation (b))

#define TEST_COMPARE_EQUAL(a, b) TEST_COMPARE_GENERIC(a, b, ==, !=)
#define TEST_COMPARE_NOT_EQUAL(a, b) TEST_COMPARE_GENERIC(a, b, !=, ==)
#define TEST_COMPARE_GREATER(a, b) TEST_COMPARE_GENERIC(a, b, >, <=)
#define TEST_COMPARE_GREATER_EQ(a, b) TEST_COMPARE_GENERIC(a, b, >=, <)
#define TEST_COMPARE_LESS(a, b) TEST_COMPARE_GENERIC(a, b, <, >=)
#define TEST_COMPARE_LESS_EQ(a, b) TEST_COMPARE_GENERIC(a, b, <=, >)

#define TEST_COMPARE_CLOSE(a, b, epsilon) \
    do { \
        ::amethyst::test::test_information local_info = TEST_INFORMATION("close_" + std::to_string(__LINE__)); \
        ::amethyst::test::current_test()->test_started(local_info); \
        const auto aaaa = (a); \
        const auto bbbb = (b); \
        if ((bbbb - aaaa) < epsilon && (aaaa - bbbb) < epsilon) \
        { \
            ::amethyst::test::current_test()->test_passed(local_info); \
        } \
        else \
        { \
            ::amethyst::test::current_test()->test_failed(local_info, ::amethyst::string_format(" test failed for " #a "~=" #b ": (%1 != %2)", aaaa, bbbb)); \
        } \
    } while (0)

#define TEST_CLOSE(a,b) TEST_COMPARE_CLOSE(a,b,AMETHYST_EPSILON)
#define TEST_XY_EQUALS(a, p, q) TEST_COMPARE_EQUAL((a).x(), p); TEST_COMPARE_EQUAL((a).y(), q);
#define TEST_XY_CLOSE(a, p, q) TEST_CLOSE((a).x(), p); TEST_CLOSE((a).y(), q);
#define TEST_XYZ_EQUALS(a, p, q, r) TEST_COMPARE_EQUAL((a).x(), p); TEST_COMPARE_EQUAL((a).y(), q); TEST_COMPARE_EQUAL((a).z(), r);
#define TEST_XYZ_CLOSE(a, p, q, r) TEST_CLOSE((a).x(), p); TEST_CLOSE((a).y(), q); TEST_CLOSE((a).z(), r);

#define TEST_EXCEPTION_GENERIC_TRYBLOCK(tryblock, catchtype, catchblock, catchallblock) \
    do { \
        ::amethyst::test::test_information local_info = TEST_INFORMATION(::amethyst::string_format("%1_exception_%2", ::amethyst::test::current_test()->test_name, __LINE__)); \
        ::amethyst::test::current_test()->test_started(local_info); \
        try { tryblock } \
        catch (const ::amethyst::test::test_aborted_exception& e) \
        { \
            ::amethyst::test::current_test()->test_failed(e.info, ::amethyst::string_format("(noticed on %1:%2) Aborted: %3", __FILE__, __LINE__, e.reason)); \
            throw; \
        } \
        catch (const catchtype& ex) { catchblock } \
        catch (...) { catchallblock } \
    } while (0)

#define TEST_EXCEPTION_THROW(exception_test_body) \
    TEST_EXCEPTION_GENERIC_TRYBLOCK( \
        { exception_test_body; ::amethyst::test::current_test()->test_failed(local_info, ::amethyst::string_format("Test did not throw for \"%1\"", #exception_test_body)); }, \
        ::amethyst::test::never_thrown_exception, \
        { (void)ex; }, \
        { ::amethyst::test::current_test()->test_passed(local_info); } \
        )

#define TEST_EXCEPTION_THROW_SPECIFIC(exception_test_body, exception_type) \
    TEST_EXCEPTION_GENERIC_TRYBLOCK( \
        { exception_test_body; ::amethyst::test::current_test()->test_failed(local_info, ::amethyst::string_format("Test did not throw for \"%1\"", #exception_test_body)); }, \
        exception_type, \
        { (void)ex; ::amethyst::test::current_test()->test_passed(local_info); }, \
        { ::amethyst::test::current_test()->test_failed(local_info, ::amethyst::string_format("Caught unexpected exception (not %1) in \"%2\"", #exception_type, #exception_test_body)); } \
        )

#define TEST_EXCEPTION_NOTHROW(exception_test_body) \
    TEST_EXCEPTION_GENERIC_TRYBLOCK( \
        { exception_test_body; ::amethyst::test::current_test()->test_passed(local_info); }, \
        ::amethyst::test::never_thrown_exception, \
        { ::amethyst::test::current_test()->test_failed(local_info, ::amethyst::string_format("Test threw exception in \"%1\": %2", #exception_test_body, amethyst::test::what(ex))); }, \
        { ::amethyst::test::current_test()->test_failed(local_info, ::amethyst::string_format("Test threw exception in \"%1\"", #exception_test_body)); } \
        )

#define TEST_EXCEPTION_NOTHROW_SPECIFIC(exception_test_body, exception_type) \
    TEST_EXCEPTION_GENERIC_TRYBLOCK( \
        { exception_test_body; ::amethyst::test::current_test()->test_passed(local_info); }, \
        exception_type, \
        { ::amethyst::test::current_test()->test_failed(local_info, ::amethyst::string_format("Test threw the disallowed exception (%1) in \"%2\": %3", #exception_type, #exception_test_body, amethyst::test::what(ex))); }, \
        { ::amethyst::test::current_test()->test_passed(local_info); } \
        )
