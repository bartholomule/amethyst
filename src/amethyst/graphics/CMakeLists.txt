add_library(amethyst_graphics
	alpha_triangle_2d.hpp
	base_camera.hpp
	capabilities.hpp
	capabilities.cpp
	conditional_value.hpp
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
	noise.hpp
	pinhole_camera.hpp
	png_io.hpp
	png_io.cpp
	ppm_io.hpp
	raster.hpp
	ray_parameters.hpp
	requirements.hpp
	requirements.cpp
	rgbcolor.hpp
	samplegen1d.hpp
	samplegen2d.hpp
	stb_image.h
	stb_image_helper.cpp
	stb_image_helper.hpp
	stb_image_write.h
	tga_io.hpp
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
