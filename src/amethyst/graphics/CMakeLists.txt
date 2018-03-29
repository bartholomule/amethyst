add_library(amethyst_graphics
	aggregate.hpp
	alpha_triangle_2d.hpp
	base_camera.hpp
	capabilities.hpp
	capabilities.cpp
	conditional_value.hpp
	disc.hpp
	dynamic_sphere.hpp
	filter3d.h
	image.hpp
	image_converter.hpp
	image_converter.cpp
	image_io.hpp
	image_loader.hpp
	image_loader.cpp
	interpolated_value.hpp
	intersection_info.hpp
	marble_texture.hpp
	material.hpp
	noise.hpp
	noise_texture.hpp
	pinhole_camera.hpp
	plane.hpp
	ppm_io.hpp
	raster.hpp
	ray_parameters.hpp
	rectangle.hpp
	requirements.hpp
	requirements.cpp
	rgbcolor.hpp
	samplegen1d.hpp
	samplegen2d.hpp
	shape.hpp
	simple_texture.hpp
	solid_texture.hpp
	sphere.hpp
	surface_texture.hpp
	texture.hpp
	tga_io.hpp
	triangle.hpp
)

unit_test(test_disc LIBS amethyst_general amethyst_graphics)
unit_test(test_quaternion LIBS amethyst_general amethyst_graphics)
unit_test(test_raster LIBS amethyst_general amethyst_graphics)
unit_test(test_fd_stream LIBS amethyst_general)
unit_test(test_rgbcolor LIBS amethyst_general amethyst_graphics)
unit_test(test_triangle LIBS amethyst_general amethyst_graphics)