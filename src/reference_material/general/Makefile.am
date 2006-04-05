lib_LTLIBRARIES = libamethyst_general.la
libamethyst_general_la_SOURCES = \
base_logger.cpp \
fd_stream.cpp \
log_formatter.cpp \
string_tokenizer.cpp \
stream_reference.cpp
libamethyst_general_la_LIBADD =
libamethyst_general_la_LDFLAGS = -shared -rpath @prefix@/lib -version-info 1
#-module 

noinst_PROGRAMS = quick_vector_vs_std_vector_perf
quick_vector_vs_std_vector_perf_SOURCES = quick_vector_vs_std_vector_perf.cc
