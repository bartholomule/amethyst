#define auto_unit_test_main main
#include "test_framework/unit_test_auto.hpp"
#include "graphics/shapes/sphere.hpp"

using namespace amethyst;

namespace
{
    using point = point3<double>;
    using vec = vector3<double>;
    using info = intersection_info<double,vec>;
}

AUTO_UNIT_TEST(sphere_construction)
{
    sphere<double,vec> s1;
    TEST_XYZ_CLOSE(s1.get_center(), 0, 0, 0);
    TEST_CLOSE(s1.get_radius(), 0);

    sphere<double,vec> s2({ 1,2,3 }, 4);
    TEST_XYZ_CLOSE(s2.get_center(), 1, 2, 3);
    TEST_CLOSE(s2.get_radius(), 4);
}

AUTO_UNIT_TEST(sphere_intersection)
{
    sphere<double,vec> s1({ 0,0,0 }, 1.00001);
    sphere<double,vec> s2({ 2,0,0 }, 1);
    sphere<double,vec> s3({ -1,0,0 }, 1);
    TEST_BOOLEAN(s1.intersects(s2));
    TEST_BOOLEAN(s1.intersects(s3));
    TEST_BOOLEAN(!s2.intersects(s3));

    unit_line3<double> l1(point{0, 0, 0}, vec{ 1,0,0 });
    info i1;
    intersection_requirements r;
    r.force_first_only(true);
    TEST_BOOLEAN(s1.intersects_line(l1, i1, r));
    TEST_XYZ_CLOSE(i1.get_first_point(), -1.00001, 0, 0);

    unit_line3<double> l2(point{ 10,0,0 }, vec(-1, 0, 0));
    info i2;
    TEST_BOOLEAN(s1.intersects_line(l2, i2, r));
    TEST_XYZ_CLOSE(i2.get_first_point(), 1.00001, 0, 0);

    // LOTS MORE IS NEEDED
}