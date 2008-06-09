SUBDIRS = general
EXTRA_DIST = 
INCLUDES = -I@top_srcdir@/include -I@top_srcdir@/src/reference_material -I@top_srcdir@/src/reference_material/graphics -I@top_srcdir@/src/reference_material/general -I@top_srcdir@/src/reference_material/math

rrt_figures = figure_3_1 figure_4_5 figure_4_10
cs5600_programs = cs5600_program01 cs5600_program02 cs5600_program03 cs5600_program04

bin_PROGRAMS = $(rrt_figures) $(cs5600_programs)

common_sources = graphics/requirements.cpp graphics/capabilities.cpp graphics/image_converter.cpp

figure_3_1_SOURCES = figure_3_1.cpp $(common_sources)
figure_4_5_SOURCES = figure_4_5.cpp $(common_sources)
figure_4_10_SOURCES = figure_4_10.cpp $(common_sources)

cs5600_program01_SOURCES = cs5600_program01.cpp graphics/image_converter.cpp
cs5600_program02_SOURCES = cs5600_program02.cpp graphics/image_converter.cpp
cs5600_program03_SOURCES = cs5600_program03.cpp graphics/image_converter.cpp
cs5600_program04_SOURCES = cs5600_program04.cpp graphics/image_converter.cpp

CXXFLAGS=-O9
CFLAGS=-O9
