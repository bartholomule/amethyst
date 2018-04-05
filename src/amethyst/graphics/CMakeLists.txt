add_library(amethyst_graphics
	aggregate.hpp
	alpha_triangle_2d.hpp
	base_camera.hpp
	capabilities.hpp
	capabilities.cpp
	conditional_value.hpp
	disc.hpp
	dynamic_sphere.hpp
	filter3d.hpp
	image.hpp
	image_converter.hpp
	image_converter.cpp
	image_io.hpp
	image_loader.hpp
	image_loader.cpp
	interpolated_value.hpp
	interpolated_value.cpp
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

function(graphics_test name)
	unit_test(${name} LIBS amethyst_general amethyst_graphics)
endfunction()

graphics_test(test_disc)
graphics_test(test_quaternion)
graphics_test(test_raster)
graphics_test(test_rgbcolor)
graphics_test(test_triangle)
graphics_test(test_sphere)

graphics_test(test_fd_stream LIBS amethyst_general)
