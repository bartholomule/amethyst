SUBDIRS = general graphics
EXTRA_DIST =
INCLUDES = -I@top_srcdir@/include

rrt_figures = figure_3_1 figure_4_5 figure_4_10 figure_5_7
cs5600_programs = cs5600_program01 cs5600_program02 cs5600_program03 cs5600_program04 genetic_triangles

bin_PROGRAMS = $(rrt_figures) $(cs5600_programs)


figure_3_1_SOURCES = figure_3_1.cpp
figure_4_5_SOURCES = figure_4_5.cpp
figure_4_10_SOURCES = figure_4_10.cpp
figure_5_7_SOURCES = figure_5_7.cpp

LDADD = -Lgraphics -lamethyst_graphics -Lgeneral -lamethyst_general

cs5600_program01_SOURCES = cs5600_program01.cpp
cs5600_program02_SOURCES = cs5600_program02.cpp
cs5600_program03_SOURCES = cs5600_program03.cpp
cs5600_program04_SOURCES = cs5600_program04.cpp
genetic_triangles_SOURCES = genetic_triangles.cpp
