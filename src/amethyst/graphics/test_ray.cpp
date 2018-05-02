#define auto_unit_test_main main
#include "test_framework/unit_test_auto.hpp"
#include "graphics/ray_parameters.hpp"
#include "graphics/iors.hpp"

using namespace amethyst;

namespace
{
	using point = point3<double>;
	using vec = vector3<double>;
	using info = intersection_info<double, vec>;
}

AUTO_UNIT_TEST(test_critical_angle)
{
	TEST_CLOSE(critical_angle<double>(1, 1), std::numeric_limits<double>::infinity());
	TEST_CLOSE(critical_angle<double>(1, 1.333), std::numeric_limits<double>::infinity());
	// water -> air =~ 48.6 degrees.
	TEST_CLOSE(critical_angle<double>(iors<double>::water, iors<double>::air), 0.848678152294);
	// glass -> air =~ 41.15 degrees
	TEST_CLOSE(critical_angle<double>(iors<double>::glass, iors<double>::air), 0.718275894237);
}


#define TEST_REFLECT_VECTOR(v1, n, v2) do { \
	vec reflected; \
	TEST_BOOLEAN(reflect(v1, n, reflected)); \
	TEST_XYZ_CLOSE(reflected, v2.x(), v2.y(), v2.z()); \
} while(0)

AUTO_UNIT_TEST(test_reflection)
{
	// Straight on, normal facing out.
	TEST_REFLECT_VECTOR(vec(1, 0, 0), vec(-1, 0, 0), vec(-1, 0, 0));
	TEST_REFLECT_VECTOR(vec(0, 1, 0), vec(0, -1, 0), vec(0, -1, 0));
	TEST_REFLECT_VECTOR(vec(0, 0, 1), vec(0, 0, -1), vec(0, 0, -1));

	// Straight on, normal same direction.
	TEST_REFLECT_VECTOR(vec(1, 0, 0), vec(1, 0, 0), vec(-1, 0, 0));
	TEST_REFLECT_VECTOR(vec(0, 1, 0), vec(0, 1, 0), vec(0, -1, 0));
	TEST_REFLECT_VECTOR(vec(0, 0, 1), vec(0, 0, 1), vec(0, 0, -1));
}

#define TEST_ANGLE_CLOSE(v1, v2, theta) TEST_CLOSE(acos(dotprod(unit(v1), unit(v2))), theta)

#define TEST_REFRACT_VECTOR(v1, n, n1, n2, v2, is_entering) do { \
	bool entering; vec refracted; \
	TEST_BOOLEAN(refract(unit(v1), unit(n), n1, n2, refracted, entering)); \
	TEST_XYZ_CLOSE(refracted, v2.x(), v2.y(), v2.z()); \
	TEST_BOOLEAN(entering == is_entering); \
} while(0)

#define TEST_REFRACT_ANGLE(v1, n, n1, n2, theta, is_entering) do { \
	bool entering; vec refracted; \
	TEST_BOOLEAN(refract(unit(v1), unit(n), n1, n2, refracted, entering)); \
	TEST_ANGLE_CLOSE(refracted, n, theta); \
	TEST_BOOLEAN(entering == is_entering); \
} while(0)

#define TEST_REFRACT_ANGLE_AND_VECTOR(v1, n, n1, n2, v2, theta, is_entering) \
	TEST_REFRACT_VECTOR(v1, n, n1, n2, v2, is_entering); \
	TEST_REFRACT_ANGLE(v1, n, n1, n2, theta, is_entering);

AUTO_UNIT_TEST(test_refraction)
{
	// Straight on, normal facing out.  Ray does not bend.
	TEST_REFRACT_ANGLE_AND_VECTOR(vec(1, 0, 0), vec(-1, 0, 0), 1.0, 1.5, vec(1, 0, 0), M_PI, true);
	TEST_REFRACT_ANGLE_AND_VECTOR(vec(0, 1, 0), vec(0, -1, 0), 1.0, 1.5, vec(0, 1, 0), M_PI, true);
	TEST_REFRACT_ANGLE_AND_VECTOR(vec(0, 0, 1), vec(0, 0, -1), 1.0, 1.5, vec(0, 0, 1), M_PI, true);
	// Straight on, normal same direction.  Ray does not bend.
	TEST_REFRACT_ANGLE_AND_VECTOR(vec(1, 0, 0), vec(1, 0, 0), 1.0, 1.5, vec(1, 0, 0), 0, false);
	TEST_REFRACT_ANGLE_AND_VECTOR(vec(0, 1, 0), vec(0, 1, 0), 1.0, 1.5, vec(0, 1, 0), 0, false);
	TEST_REFRACT_ANGLE_AND_VECTOR(vec(0, 0, 1), vec(0, 0, 1), 1.0, 1.5, vec(0, 0, 1), 0, false);


	// 45 degrees
	// No IOR difference = no bending
	TEST_REFRACT_ANGLE_AND_VECTOR(vec(1, 1, 0), vec(0, 0, -1), 1.0, 1.0, unit(vec(1, 1, 0)), M_PI / 2, true);
	// TEST_REFRACT_ANGLE_AND_VECTOR(vec(1, 1, 0), vec(0, 0, -1), 1.0, 2.0, unit(vec(1, 1, 0)), M_PI * 3 / 4, true);

	// FIXME!
}
