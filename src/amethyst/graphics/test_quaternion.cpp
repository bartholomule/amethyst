#define auto_unit_test_main main
#include "test_framework/unit_test_auto.hpp"
#include "math/quaternion.hpp"
#include "math/vector3.hpp"
#include <cmath>
#include <iostream>

namespace
{
    using namespace amethyst;
    typedef float base_type;
    typedef vector3<base_type> coord;
    typedef quaternion<base_type> quat;
    const base_type epsilon = 0.0001f;
}

AUTO_UNIT_TEST(quaternion_unit)
{
    quat i(0, coord(1, 0, 0));
    quat j(0, coord(0, 1, 0));
    quat k(0, coord(0, 0, 1));

    TEST_COMPARE_CLOSE(-1, quat(i * i).getReal(), epsilon);
    TEST_COMPARE_CLOSE(-1, quat(j * j).getReal(), epsilon);
    TEST_COMPARE_CLOSE(-1, quat(k * k).getReal(), epsilon);

    TEST_COMPARE_CLOSE(0, length(quat(i * i).getImag()), epsilon);
    TEST_COMPARE_CLOSE(0, length(quat(j * j).getImag()), epsilon);
    TEST_COMPARE_CLOSE(0, length(quat(k * k).getImag()), epsilon);
}

AUTO_UNIT_TEST(quaternion_norm)
{
    quat q = makeUnitQuaternion<base_type>(45, coord(0, 0, 1));
    TEST_COMPARE_CLOSE(1, q.norm(), epsilon);
}

AUTO_UNIT_TEST(quaternion_rotate)
{
    quat q = makeUnitQuaternionDegrees<base_type>(45, coord(0, 0, 1));
    coord v(1, 0, 0);
    // 45 degrees around z.  Rotating twice should be y==1
    coord v2(quat::rotate(quat::rotate(v, q), q));
    TEST_COMPARE_CLOSE(0, v2.x(), epsilon);
    TEST_COMPARE_CLOSE(1, v2.y(), epsilon);
    TEST_COMPARE_CLOSE(0, v2.z(), epsilon);
    // Rotating 4 times should be x == -1
    coord v3(quat::rotate(quat::rotate(quat::rotate(quat::rotate(v, q), q), q), q));
    TEST_COMPARE_CLOSE(-1, v3.x(), epsilon);
    TEST_COMPARE_CLOSE(0, v3.y(), epsilon);
    TEST_COMPARE_CLOSE(0, v3.z(), epsilon);
    // Rotating 6 times should be y == -1
    coord v4(quat::rotate(quat::rotate(quat::rotate(quat::rotate(quat::rotate(quat::rotate(v, q), q), q), q), q), q));
    TEST_COMPARE_CLOSE(0, v4.x(), epsilon);
    TEST_COMPARE_CLOSE(-1, v4.y(), epsilon);
    TEST_COMPARE_CLOSE(0, v4.z(), epsilon);
    // Rotating 8 times should be x == 1
    coord v5(quat::rotate(quat::rotate(quat::rotate(quat::rotate(quat::rotate(quat::rotate(quat::rotate(quat::rotate(v, q), q), q), q), q), q), q), q));
    TEST_COMPARE_CLOSE(1, v5.x(), epsilon);
    TEST_COMPARE_CLOSE(0, v5.y(), epsilon);
    TEST_COMPARE_CLOSE(0, v5.z(), epsilon);
}

