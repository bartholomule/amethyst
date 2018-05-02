#define auto_unit_test_main main
#include "test_framework/unit_test_auto.hpp"
#include "math/quaternion.hpp"
#include "math/vector3.hpp"
#include <cmath>
#include <iostream>

namespace
{
    using namespace amethyst;
    using base_type = double;
    using coord = vector3<base_type>;
    using quat = quaternion<base_type>;
}

AUTO_UNIT_TEST(quaternion_unit)
{
    quat i(0, coord(1, 0, 0));
    quat j(0, coord(0, 1, 0));
    quat k(0, coord(0, 0, 1));

    TEST_CLOSE(-1, quat(i * i).getReal());
    TEST_CLOSE(-1, quat(j * j).getReal());
    TEST_CLOSE(-1, quat(k * k).getReal());

    TEST_CLOSE(0, length(quat(i * i).getImag()));
    TEST_CLOSE(0, length(quat(j * j).getImag()));
    TEST_CLOSE(0, length(quat(k * k).getImag()));
}

AUTO_UNIT_TEST(quaternion_norm)
{
    quat q = makeUnitQuaternion<base_type>(45, coord(0, 0, 1));
    TEST_CLOSE(1, q.norm());
}

AUTO_UNIT_TEST(quaternion_rotate)
{
    quat q = makeUnitQuaternionDegrees<base_type>(45, coord(0, 0, 1));
    coord v(1, 0, 0);
    // 45 degrees around z.  Rotating twice should be y==1
    coord v2(quat::rotate(quat::rotate(v, q), q));
    TEST_CLOSE(0, v2.x());
    TEST_CLOSE(1, v2.y());
    TEST_CLOSE(0, v2.z());
    // Rotating 4 times should be x == -1
    coord v3(quat::rotate(quat::rotate(quat::rotate(quat::rotate(v, q), q), q), q));
    TEST_CLOSE(-1, v3.x());
    TEST_CLOSE(0, v3.y());
    TEST_CLOSE(0, v3.z());
    // Rotating 6 times should be y == -1
    coord v4(quat::rotate(quat::rotate(quat::rotate(quat::rotate(quat::rotate(quat::rotate(v, q), q), q), q), q), q));
    TEST_CLOSE(0, v4.x());
    TEST_CLOSE(-1, v4.y());
    TEST_CLOSE(0, v4.z());
    // Rotating 8 times should be x == 1
    coord v5(quat::rotate(quat::rotate(quat::rotate(quat::rotate(quat::rotate(quat::rotate(quat::rotate(quat::rotate(v, q), q), q), q), q), q), q), q));
    TEST_CLOSE(1, v5.x());
    TEST_CLOSE(0, v5.y());
    TEST_CLOSE(0, v5.z());
}

