#define auto_unit_test_main main
#include "test_framework/unit_test_auto.hpp"
#include "math/coord3.hpp"
#include "math/vector3.hpp"
#include "math/vector_utils.hpp"

namespace
{
    using namespace amethyst;
    using base_type = double;
}

AUTO_UNIT_TEST(coord_construction)
{
    coord3<int> c1(1);
    TEST_COMPARE_EQUAL(c1.x(), 1); TEST_COMPARE_EQUAL(c1.y(), 1); TEST_COMPARE_EQUAL(c1.z(), 1);

    coord3<int> c2(1, 2, 3);
    TEST_COMPARE_EQUAL(c2.x(), 1);
    TEST_COMPARE_EQUAL(c2.y(), 2);
    TEST_COMPARE_EQUAL(c2.z(), 3);
}

template <typename T>
void test_coord_accessors()
{
    T c1(1, 1, 1);
    TEST_XYZ_EQUALS(c1, 1, 1, 1);
    c1.set(2, 3, 4);
    TEST_XYZ_EQUALS(c1, 2, 3, 4);

    c1.x() = -1;
    TEST_XYZ_EQUALS(c1, -1, 3, 4);
    c1.y() = -2;
    TEST_XYZ_EQUALS(c1, -1, -2, 4);
    c1.z() = -3;
    TEST_XYZ_EQUALS(c1, -1, -2, -3);

    c1[0] = c1[1] = c1[2] = 99;
    TEST_XYZ_EQUALS(c1, 99, 99, 99);

    c1[0] = 10;
    TEST_XYZ_EQUALS(c1, 10, 99, 99);
    c1[1] = 20;
    TEST_XYZ_EQUALS(c1, 10, 20, 99);
    c1[2] = 30;
    TEST_XYZ_EQUALS(c1, 10, 20, 30);
}

AUTO_UNIT_TEST(coord_accessors)
{
    TEST_XYZ_EQUALS(coord3<int>(4), 4, 4, 4);
    test_coord_accessors<coord3<int>>();
    test_coord_accessors<vector3<int>>();
}

template <typename T>
void test_coord_math()
{
    const T zero(0, 0, 0);
    const T X(1, 0, 0);
    const T Y(0, 1, 0);
    const T Z(0, 0, 1);

    TEST_XYZ_CLOSE(X + X, 2, 0, 0);
    TEST_XYZ_CLOSE(X + Y, 1, 1, 0);
    TEST_XYZ_CLOSE(X + Z, 1, 0, 1);
    TEST_XYZ_CLOSE(Y + X, 1, 1, 0);
    TEST_XYZ_CLOSE(Y + Y, 0, 2, 0);
    TEST_XYZ_CLOSE(Y + Z, 0, 1, 1);
    TEST_XYZ_CLOSE(Z + X, 1, 0, 1);
    TEST_XYZ_CLOSE(Z + Y, 0, 1, 1);
    TEST_XYZ_CLOSE(Z + Z, 0, 0, 2);

    TEST_XYZ_CLOSE(X - X, 0, 0, 0);
    TEST_XYZ_CLOSE(X - Y, 1, -1, 0);
    TEST_XYZ_CLOSE(X - Z, 1, 0, -1);
    TEST_XYZ_CLOSE(Y - X, -1, 1, 0);
    TEST_XYZ_CLOSE(Y - Y, 0, 0, 0);
    TEST_XYZ_CLOSE(Y - Z, 0, 1, -1);
    TEST_XYZ_CLOSE(Z - X, -1, 0, 1);
    TEST_XYZ_CLOSE(Z - Y, 0, -1, 1);
    TEST_XYZ_CLOSE(Z - Z, 0, 0, 0);

    TEST_XYZ_CLOSE(X * 2, 2, 0, 0);
    TEST_XYZ_CLOSE(Y * 2, 0, 2, 0);
    TEST_XYZ_CLOSE(Z * 2, 0, 0, 2);
    TEST_XYZ_CLOSE(2 * X, 2, 0, 0);
    TEST_XYZ_CLOSE(2 * Y, 0, 2, 0);
    TEST_XYZ_CLOSE(2 * Z, 0, 0, 2);

    TEST_XYZ_CLOSE(X / 4, 0.25, 0, 0);
    TEST_XYZ_CLOSE(Y / 4, 0, 0.25, 0);
    TEST_XYZ_CLOSE(Z / 4, 0, 0, 0.25);

    TEST_XYZ_CLOSE(2 * X + 4 * Y + 15 * Z, 2, 4, 15);
}

AUTO_UNIT_TEST(coord_math)
{
    test_coord_math<coord3<double>>();
    test_coord_math<vector3<double>>();
}

template <typename T>
void test_coord_functions()
{
    T c1;
    c1.set(1, 0, 0);
    TEST_CLOSE(length(c1), 1);
    c1.set(0, 1, 0);
    TEST_CLOSE(length(c1), 1);
    c1.set(0, 0, 1);
    TEST_CLOSE(length(c1), 1);

    const T X(1, 0, 0);
    const T Y(0, 1, 0);
    const T Z(0, 0, 1);
    const T X2 = 2 * X;
    const T Y2 = 2 * Y;
    const T Z2 = 2 * Z;
    TEST_CLOSE(dotprod(X, X), 1);
    TEST_CLOSE(dotprod(X, Y), 0);
    TEST_CLOSE(dotprod(X, Z), 0);
    TEST_CLOSE(dotprod(Y, Y), 1);
    TEST_CLOSE(dotprod(Y, Z), 0);
    TEST_CLOSE(dotprod(Z, Z), 1);
    TEST_CLOSE(dotprod(X2, X2), 4);
    TEST_CLOSE(dotprod(Y2, Y2), 4);
    TEST_CLOSE(dotprod(Z2, Z2), 4);
}

AUTO_UNIT_TEST(coord_functions)
{
    test_coord_functions<coord3<double>>();
    test_coord_functions<vector3<double>>();
}


AUTO_UNIT_TEST(abs_index)
{
    using c = coord3<double>;
    TEST_COMPARE_EQUAL(min_abs_index(c(1, 0, 0)), 1);
    TEST_COMPARE_EQUAL(min_abs_index(c(1, 1, 0)), 2);
    TEST_COMPARE_EQUAL(min_abs_index(c(1, 1, -1)), 0);
    TEST_COMPARE_EQUAL(min_abs_index(c(0, 1, -1)), 0);
}

AUTO_UNIT_TEST(test_perp_vector)
{
    vector3<double> x = { 1,0,0 };
    vector3<double> y = { 0,1,0 };
    vector3<double> z = { 0,0,1 };

    auto px = perp_vector(x);
    TEST_XYZ_CLOSE(px, 0, 0, -1);
    TEST_CLOSE(dotprod(x, px), 0);

    auto py = perp_vector(y);
    TEST_XYZ_CLOSE(py, 0, 0, -1);
    TEST_CLOSE(dotprod(y, py), 0);

    auto pz = perp_vector(z);
    TEST_XYZ_CLOSE(pz, 0, 1, 0);
    TEST_CLOSE(dotprod(z, pz), 0);
}

#define CHECK_ANGLE(a, b, theta) TEST_CLOSE(acos(sqrt(dotprod(unit(a),unit(b)))), theta)

AUTO_UNIT_TEST(test_vector_angle)
{
    using vec = vector3<double>;
    CHECK_ANGLE(vec(1, 0, 0), vec(0, 1, 0), M_PI / 2);
    CHECK_ANGLE(vec(1, 0, 0), vec(0, 0, 1), M_PI / 2);
    CHECK_ANGLE(vec(0, 1, 0), vec(1, 0, 0), M_PI / 2);
    CHECK_ANGLE(vec(0, 1, 0), vec(0, 0, 1), M_PI / 2);
    CHECK_ANGLE(vec(0, 0, 1), vec(1, 0, 0), M_PI / 2);
    CHECK_ANGLE(vec(0, 0, 1), vec(0, 1, 0), M_PI / 2);

    CHECK_ANGLE(vec(1, 0, 0), vec(0, 1, 1), M_PI / 2);
    CHECK_ANGLE(vec(1, 1, 0), vec(0, 0, 1), M_PI / 2);
    CHECK_ANGLE(vec(0, 1, 0), vec(1, 0, 1), M_PI / 2);
    CHECK_ANGLE(vec(0, 1, 1), vec(1, 0, 0), M_PI / 2);
    CHECK_ANGLE(vec(0, 0, 1), vec(1, 1, 0), M_PI / 2);

}

}
