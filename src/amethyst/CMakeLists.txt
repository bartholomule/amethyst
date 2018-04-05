set(MAIN_SRCDIR ${CMAKE_CURRENT_SOURCE_DIR}/..)
include_directories(${MAIN_SRCDIR})
set(AMETHYST_SOURCE ${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${AMETHYST_SOURCE})

add_subdirectory(test_framework)

function(unit_test name)
	add_test_exe(NAME ${name} SRCS ${name}.cpp LIBS amethyst_test ${ARGN})
endfunction()

add_subdirectory(general)
add_subdirectory(math)
add_subdirectory(graphics)

function(compile_example name)
	add_exe(
		NAME
			${name}
		SRCS
			${name}.cpp
		LIBS
			amethyst_general
			amethyst_graphics
	)
endfunction()

compile_example(figure_3_1)
compile_example(figure_4_5)
compile_example(figure_4_10)
compile_example(figure_5_7)
configure_file("earth.ppm" "earth.ppm" COPYONLY)
configure_file("uv_grid_400x400.ppm" "uv_grid_400x400.ppm" COPYONLY)
compile_example(figure_6_2)
compile_example(cs5600_program01)
compile_example(cs5600_program02)
configure_file("program3.txt" "program3.txt" COPYONLY)
compile_example(cs5600_program03)
compile_example(cs5600_program04)
compile_example(genetic_triangles)
compile_example(load_and_save)

compile_example(rtiow_01_gradient)
compile_example(rtiow_02_sphere)
compile_example(rtiow_03_sphere_normal)
