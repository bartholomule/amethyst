// g++ -g test.cpp -o test -lm -W -Wall -I.

#if 0
#include <graphics/shape.hpp>
#include <graphics/sphere.hpp>
#include <graphics/plane.hpp>
#endif

#include <math/quaternion.hpp>

#include <general/string_format.hpp>

#include <iostream>

typedef amethyst::point3<double> point;
typedef amethyst::vector3<double> vec;


using amethyst::string_format;

int main(int, char**)
{
#if 0
    amethyst::sphere<double> sph1(point(0, 2, 0), 1);
    amethyst::sphere<double> sph2(point(0, 2, 0), 2);
    amethyst::plane<double> pla1(point(0, 0, 0), vec(0, 1, 0));
    amethyst::plane<double> pla2(point(0, 1, 0), vec(0, 1, 0));
    amethyst::plane<double> pla3(point(0, 0, 0), vec(0, 1, 0.000045));

    cout << "sph1 intersecting with pla1=" << sph1.intersects(pla1) << endl;
    cout << "sph2 intersecting with pla1=" << sph2.intersects(pla1) << endl;

    cout << "sph1 intersecting with pla2=" << sph1.intersects(pla2) << endl;
    cout << "sph2 intersecting with pla2=" << sph2.intersects(pla2) << endl;

    cout << "sph1 intersecting with sph2=" << sph1.intersects(sph2) << endl;
    cout << "sph2 intersecting with sph2=" << sph2.intersects(sph1) << endl;

    cout << "pla1 intersecting with pla1=" << pla1.intersects(pla1) << endl;

    cout << "pla1 intersecting with pla2=" << pla1.intersects(pla2) << endl;
    cout << "pla2 intersecting with pla1=" << pla2.intersects(pla1) << endl;

    cout << "pla1 intersecting with pla3=" << pla1.intersects(pla3) << endl;
    cout << "pla2 intersecting with pla3=" << pla2.intersects(pla3) << endl;

    cout << string_format("This should show a number: %1", 100) << endl;
    cout << string_format("This should be a percent: %%", 0) << endl;
    cout << string_format("This should be two percents: %%%%", 0) << endl;
    cout << string_format("This is in hex: %x1", 0xBADF00D) << endl;
    cout << string_format("Multiple output of the same arg: %1 %1 %1 (%1%1)", 7) << endl;
    cout << string_format("Invalid argument number: %99", 10) << endl;

    cout << string_format("Here's a string (char*): \"%1\"", "Hello!") << endl;
    cout << string_format("Here's a string: \"%1\"", string("Hello!")) << endl;

    cout << string_format("Two different arguments: %1 %2", 1, 2) << endl;
    cout << string_format("Two different arguments: %2 %1", 1, 2) << endl;

    cout << string_format("Three different arguments: %1 %2 %3", 1, 2, 3) << endl;
    cout << string_format("Four different arguments: %1 %2 %3 %4", 1, 2, 3, 4) << endl;
    cout << string_format("Five different arguments: %1 %2 %3 %4 %5", 1, 2, 3, 4, 5) << endl;
    cout << string_format("Six different arguments: %1 %2 %3 %4 %5 %6", 1, 2, 3, 4, 5, 6) << endl;

    cout << string_format("Six different argument types: %1 %2 %3 %4 %5 %6", 1, "2", 3.0, string("4"), false, vec(1, 2, 3)) << endl;

    cout << pla1 << endl;
    cout << sph1 << endl;
#endif // 0

    typedef amethyst::quaternion<double> quat;

    quat foo(1, quat::VECTOR(1, 0, 0));
    cout << foo << endl;
    cout << foo * foo << endl;
    cout << foo * foo * foo << endl;

    quat i(0, quat::VECTOR(1, 0, 0));
    cout << i << endl;
    cout << i * i << endl;

    return 0;
}
