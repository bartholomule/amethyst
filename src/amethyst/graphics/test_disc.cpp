#define auto_unit_test_main main
#include "test_framework/unit_test_auto.hpp"
#include "graphics/shapes/disc.hpp"
#include <cmath>
#include <iostream>

namespace
{
    typedef double base_type;
    typedef amethyst::vector3<base_type> vec3;
    typedef amethyst::point3<base_type> point3;
    typedef amethyst::disc<base_type,vec3> disc;
    typedef amethyst::sphere<base_type,vec3> sphere;
    typedef amethyst::plane<base_type,vec3> plane;
}

// Sanity checks
AUTO_UNIT_TEST(test_sphere_sphere)
{
    //	TEST_BOOLEAN();
    vec3 vx(1, 0, 0);
    vec3 vy(0, 1, 0);
    vec3 vz(0, 0, 1);

    point3 origin(0, 0, 0);

    TEST_BOOLEAN(sphere(origin, 1).intersects(sphere(origin, 1)));
    TEST_BOOLEAN(sphere(origin, 2).intersects(sphere(origin, 1)));
    TEST_BOOLEAN(sphere(origin, 1).intersects(sphere(origin, 2)));
    TEST_BOOLEAN(sphere(origin, 1).intersects(sphere(origin + vx, 1)));
    TEST_BOOLEAN(sphere(origin, 1).intersects(sphere(origin + vy, 1)));
    TEST_BOOLEAN(sphere(origin, 1).intersects(sphere(origin + vz, 1)));
}


// Test a disc intersecting a sphere.
AUTO_UNIT_TEST(test_disc_sphere)
{
    //	TEST_BOOLEAN();
    vec3 vx(1, 0, 0);
    vec3 vy(0, 1, 0);
    vec3 vz(0, 0, 1);

    base_type e = 2 * AMETHYST_EPSILON;

    vec3 vxe(1 + e, 0, 0);
    vec3 vye(0, 1 + e, 0);
    vec3 vze(0, 0, 1 + e);

    point3 origin(0, 0, 0);

    // Cocentric.
    TEST_BOOLEAN(disc(origin, 1, vx).intersects(sphere(origin, 1)));
    TEST_BOOLEAN(disc(origin, 1, vy).intersects(sphere(origin, 1)));
    TEST_BOOLEAN(disc(origin, 1, vz).intersects(sphere(origin, 1)));

    TEST_BOOLEAN(disc(origin, 2, vx).intersects(sphere(origin, 1)));
    TEST_BOOLEAN(disc(origin, 2, vy).intersects(sphere(origin, 1)));
    TEST_BOOLEAN(disc(origin, 2, vz).intersects(sphere(origin, 1)));

    TEST_BOOLEAN(disc(origin, 1, vx).intersects(sphere(origin, 2)));
    TEST_BOOLEAN(disc(origin, 1, vy).intersects(sphere(origin, 2)));
    TEST_BOOLEAN(disc(origin, 1, vz).intersects(sphere(origin, 2)));

    // Intersect within epsilon (outside disc)
    TEST_BOOLEAN(disc(origin, 1, vx).intersects(sphere(origin + vx, 1)));
    TEST_BOOLEAN(disc(origin, 1, vx).intersects(sphere(origin - vx, 1)));
    TEST_BOOLEAN(disc(origin, 1, vx).intersects(sphere(origin + 2 * vy, 1)));
    TEST_BOOLEAN(disc(origin, 1, vx).intersects(sphere(origin - 2 * vy, 1)));
    TEST_BOOLEAN(disc(origin, 1, vx).intersects(sphere(origin + 2 * vz, 1)));
    TEST_BOOLEAN(disc(origin, 1, vx).intersects(sphere(origin - 2 * vz, 1)));

    TEST_BOOLEAN(disc(origin, 1, vy).intersects(sphere(origin + 2 * vx, 1)));
    TEST_BOOLEAN(disc(origin, 1, vy).intersects(sphere(origin - 2 * vx, 1)));
    TEST_BOOLEAN(disc(origin, 1, vy).intersects(sphere(origin + vy, 1)));
    TEST_BOOLEAN(disc(origin, 1, vy).intersects(sphere(origin - vy, 1)));
    TEST_BOOLEAN(disc(origin, 1, vy).intersects(sphere(origin + 2 * vz, 1)));
    TEST_BOOLEAN(disc(origin, 1, vy).intersects(sphere(origin - 2 * vz, 1)));

    TEST_BOOLEAN(disc(origin, 1, vz).intersects(sphere(origin + 2 * vx, 1)));
    TEST_BOOLEAN(disc(origin, 1, vz).intersects(sphere(origin - 2 * vx, 1)));
    TEST_BOOLEAN(disc(origin, 1, vz).intersects(sphere(origin + 2 * vy, 1)));
    TEST_BOOLEAN(disc(origin, 1, vz).intersects(sphere(origin - 2 * vy, 1)));
    TEST_BOOLEAN(disc(origin, 1, vz).intersects(sphere(origin + vz, 1)));
    TEST_BOOLEAN(disc(origin, 1, vz).intersects(sphere(origin - vz, 1)));

    // No intersection
    TEST_BOOLEAN(!disc(origin, 1, vx).intersects(sphere(origin + vxe, 1)));
    TEST_BOOLEAN(!disc(origin, 1, vx).intersects(sphere(origin - vxe, 1)));
    TEST_BOOLEAN(!disc(origin, 1, vx).intersects(sphere(origin + (vy + vye), 1)));
    TEST_BOOLEAN(!disc(origin, 1, vx).intersects(sphere(origin - (vy + vye), 1)));
    TEST_BOOLEAN(!disc(origin, 1, vx).intersects(sphere(origin + (vz + vze), 1)));
    TEST_BOOLEAN(!disc(origin, 1, vx).intersects(sphere(origin - (vz + vze), 1)));

    TEST_BOOLEAN(!disc(origin, 1, vy).intersects(sphere(origin + (vx + vxe), 1)));
    TEST_BOOLEAN(!disc(origin, 1, vy).intersects(sphere(origin - (vx + vxe), 1)));
    TEST_BOOLEAN(!disc(origin, 1, vy).intersects(sphere(origin + vye, 1)));
    TEST_BOOLEAN(!disc(origin, 1, vy).intersects(sphere(origin - vye, 1)));
    TEST_BOOLEAN(!disc(origin, 1, vy).intersects(sphere(origin + (vz + vze), 1)));
    TEST_BOOLEAN(!disc(origin, 1, vy).intersects(sphere(origin - (vz + vze), 1)));

    TEST_BOOLEAN(!disc(origin, 1, vz).intersects(sphere(origin + (vx + vxe), 1)));
    TEST_BOOLEAN(!disc(origin, 1, vz).intersects(sphere(origin - (vx + vxe), 1)));
    TEST_BOOLEAN(!disc(origin, 1, vz).intersects(sphere(origin + (vy + vye), 1)));
    TEST_BOOLEAN(!disc(origin, 1, vz).intersects(sphere(origin - (vy + vye), 1)));
    TEST_BOOLEAN(!disc(origin, 1, vz).intersects(sphere(origin + vze, 1)));
    TEST_BOOLEAN(!disc(origin, 1, vz).intersects(sphere(origin - vze, 1)));
}

AUTO_UNIT_TEST(test_ray_disc)
{
    // FIXME! ray-disc intersections
}


