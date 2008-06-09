#define auto_unit_test_main main
#include "unit_test_auto.hpp"

#include <iostream>

AUTO_UNIT_TEST(sam)
{
	std::cerr << "An auto unit test (sam) really ran." << std::endl;
	TEST_COMPARE_EQUAL(10, 5 + 5);
	TEST_EXCEPTION_NOTHROW((2 + 2));
}


AUTO_UNIT_TEST(bob)
{
	std::cerr << "An auto unit test (bob) really ran." << std::endl;
	TEST_COMPARE_EQUAL('A', int('A'));
	TEST_EXCEPTION_THROW(throw int(2));
}


AUTO_UNIT_TEST(joe)
{
	std::cerr << "An auto unit test (joe) really ran." << std::endl;
	int x = 1;
	int y = 1;
	TEST_COMPARE_EQUAL(x, y);
	TEST_COMPARE_EQUAL(x, y);
	TEST_COMPARE_EQUAL(x, y);
	TEST_COMPARE_EQUAL(x, y);
	TEST_COMPARE_EQUAL(x, y);
	TEST_COMPARE_EQUAL(x, y);
}


AUTO_UNIT_TEST(bad_tests)
{
	std::cerr << "These tests should all fail." << std::endl;

	int x = 1;
	int y = 2;
	TEST_COMPARE_EQUAL(x, y);

	TEST_EXCEPTION_NOTHROW(throw int(2));
	TEST_EXCEPTION_THROW_SPECIFIC(throw int(2), double);
	TEST_EXCEPTION_NOTHROW_SPECIFIC(throw int(2), int);
	TEST_EXCEPTION_THROW(AMETHYST_ABORT_TESTS("I'm tired of running."));

}
