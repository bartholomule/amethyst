SUBDIRS = general
EXTRA_DIST = 
INCLUDES = -I@top_srcdir@/include -I@top_srcdir@/src/reference_material -I@top_srcdir@/src/reference_material/graphics -I@top_srcdir@/src/reference_material/general -I@top_srcdir@/src/reference_material/math
#lib_LTLIBRARIES = libmanygames.la

# A program to test compilation/linkage of the libraries.
#bin_PROGRAMS = barf
#barf_SOURCES = barf.cpp 
#barf_LDADD = libmanygames.la

# graphics_objects = graphics/image_converter.lo
# generic_objects  = generic/extra_exceptions.lo

# graphics_libs = graphics/libmanygames_graphics.la
# generic_libs = generic/libmanygames_generic.la

# libmanygames_la_SOURCES = emptyfile.cpp
# libmanygames_la_LIBADD = $(graphics_libs) $(generic_libs)
# #$(graphics_objects) $(generic_objects)
# libmanygames_la_LDFLAGS = -module -shared -rpath @prefix@/lib 

rrt_figures = figure_3_1 figure_4_5 figure_4_10
cs5600_programs = cs5600_program01 cs5600_program02

bin_PROGRAMS = $(rrt_figures) $(cs5600_programs)

common_sources = graphics/requirements.cpp graphics/capabilities.cpp graphics/image_converter.cpp

figure_3_1_SOURCES = figure_3_1.cpp $(common_sources)
figure_4_5_SOURCES = figure_4_5.cpp $(common_sources)
figure_4_10_SOURCES = figure_4_10.cpp $(common_sources)

cs5600_program01_SOURCES = cs5600_program01.cpp graphics/image_converter.cpp
cs5600_program02_SOURCES = cs5600_program02.cpp graphics/image_converter.cpp
