# A wrapper around several target-specific executable building
# options.  This is useful for avoiding a bunch of global definitions
# for the various options.  It is also more convenient.
#
# Valid options:
#   NAME <name> -- (REQUIRED) the name of the executable (no suffix)
#   SRCS <source list> -- (REQUIRED) the list of sources for the executable
#   LIBS <lib list> -- (OPTIONAL) the list of libraries to link against
#   INCLUDES <include list> -- (OPTIONAL) the list of include directories for the target
#   PCH_HEADER <pch.h> -- (OPTIONAL) specify a header for precompiled headers
#   PCH_SOURCE <pch.cpp> -- (OPTIONAL) specify a source file for precompiled headers
# SRCS and LIBS can have prefixes for WIN, NOT_WIN, LINUX, NOT_LINUX, OSX, NOT_OSX.
#
# Examples:
#   add_binary(NAME foo TYPE EXE SRCS foo.cpp)
#   add_binary(NAME bar TYPE EXE SRCS bar.cpp INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/.. LIBS x11)
#   add_binary(NAME baz TYPE LIB LINKAGE STATIC SRCS baz.cpp NOT_WIN_SOURCES quux.cpp)
function(add_binary)
  #message("add_binary(${ARGN})")

  cmake_parse_arguments(
    PARAMS # prefix for local variables
    ""     # No boolean arguments
    "NAME;TYPE;LINKAGE;PCH_HEADER;PCH_SOURCE"
    "SRCS;LIBS;INCLUDES;WIN_SRCS;WIN_LIBS;NOT_WIN_SRCS;NOT_WIN_LIBS;LINUX_SRCS;LINUX_LIBS;NOT_LINUX_SRCS;NOT_LINUX_LIBS;OSX_SRCS;OSX_LIBS;NOT_OSX_SRCS;NOT_OSX_LIBS" # Arguments the user will (usually optionally) provide.
    ${ARGN}
  )

  if(NOT PARAMS_NAME)
    message(FATAL_ERROR "No name provided")
  endif(NOT PARAMS_NAME)

  # It is legal to have all platform-specific sources but no general sources
  # included.  Create an empty variable for them to be added below.
  if(NOT PARAMS_SRCS)
    set(PARAMS_SRCS)
  endif()

  # Combine all of the possible source listings into one list of sources.
  # Platform-specific:
  if(MSVC AND DEFINED PARAMS_WIN_SRCS AND PARAMS_WIN_SRCS)
    list(APPEND PARAMS_SRCS ${PARAMS_WIN_SRCS})
  elseif(LINUX AND DEFINED PARAMS_LINUX_SRCS AND PARAMS_LINUX_SRCS)
    list(APPEND PARAMS_SRCS ${PARAMS_LINUX_SRCS})
  elseif(APPLE AND DEFINED PARAMS_OSX_SRCS AND PARAMS_OSX_SRCS)
    list(APPEND PARAMS_SRCS ${PARAMS_OSX_SRCS})
  endif()
  # Inverted logic for platforms:
  if((NOT MSVC) AND (DEFINED PARAMS_NOT_WIN_SRCS))
    list(APPEND PARAMS_SRCS ${PARAMS_NOT_WIN_SRCS})
  endif()
  if((NOT LINUX) AND (DEFINED PARAMS_NOT_LINUX_SRCS))
    list(APPEND PARAMS_SRCS ${PARAMS_NOT_LINUX_SRCS})
  endif()
  if((NOT APPLE) AND (DEFINED PARAMS_NOT_OSX_SRCS))
    list(APPEND PARAMS_SRCS ${PARAMS_NOT_OSX_SRCS})
  endif()

  # Must have source files.
  if(NOT PARAMS_SRCS)
    message(FATAL_ERROR "No source files provided with ((NOT_)?(WIN_|LINUX_|OSX))?SRCS parameters")
  endif(NOT PARAMS_SRCS)

  # Optionally have precompiled header files
  if((DEFINED PARAMS_PCH_HEADER) AND (DEFINED PARAMS_PCH_SOURCE))
    add_precompiled_header(${PARAMS_PCH_HEADER} ${PARAMS_PCH_SOURCE} PARAMS_SRCS)
  endif()

  # if cmake supported calling functions by name, this would be much half as much code.
  if(DEFINED PARAMS_TYPE)
    if(${PARAMS_TYPE} STREQUAL "TEST-EXE")
	  message("${PARAMS_NAME}: will be a binary of type ${PARAMS_TYPE}")

      if(DEFINED PARAMS_LINKAGE)
        message(ERROR, "Linkage options not yet implemented for type TEST-EXE")
      endif()
	  add_executable(${PARAMS_NAME} ${PARAMS_SRCS})
	  enable_testing()
	  add_test(NAME ${PARAMS_NAME} COMMAND $<TARGET_FILE:${PARAMS_NAME}>)
    elseif(${PARAMS_TYPE} STREQUAL "EXE")
	  message("${PARAMS_NAME}: will be a binary of type ${PARAMS_TYPE}")

      if(DEFINED PARAMS_LINKAGE)
        message(ERROR, "Linkage options not yet implemented for type EXE")
      endif()
      add_executable(${PARAMS_NAME} ${PARAMS_SRCS})
    elseif(${PARAMS_TYPE} STREQUAL "LIB")
	  message("${PARAMS_NAME}: will be a binary of type ${PARAMS_TYPE}")

      if(DEFINED PARAMS_LINKAGE)
        add_library(${PARAMS_NAME} ${PARAMS_LINKAGE} ${PARAMS_SRCS})
      else()
        add_library(${PARAMS_NAME} ${PARAMS_SRCS})
      endif()
	else()
	  message(FATAL_ERROR "${PARAMS_NAME}: WILL NOT WORK as a ${PARAMS_TYPE}")
    endif()
  else()
    message(FATAL_ERROR "No TYPE provided for target ${PARAMS_NAME}")
  endif()

  # Target-specific include directories
  if((DEFINED PARAMS_INCLUDES) AND PARAMS_INCLUDES)
    target_include_directories(${PARAMS_NAME} PUBLIC ${PARAMS_INCLUDES})
  endif()

  # User-specified libraries.  Note that target_link_libraries is additive, so
  # they don't need to all be included on one call
  if(DEFINED PARAMS_LIBS)
    target_link_libraries(${PARAMS_NAME} ${PARAMS_LIBS})
  endif(DEFINED PARAMS_LIBS)
  if(MSVC AND (DEFINED PARAMS_WIN_LIBS) AND PARAMS_WIN_LIBS)
    target_link_libraries(${PARAMS_NAME} ${PARAMS_WIN_LIBS})
  elseif(LINUX AND (DEFINED PARAMS_LINUX_LIBS) AND PARAMS_LINUX_LIBS)
    target_link_libraries(${PARAMS_NAME} ${PARAMS_LINUX_LIBS})
  elseif(APPLE AND (DEFINED PARAMS_OSX_LIBS) AND PARAMS_OSX_LIBS)
    target_link_libraries(${PARAMS_NAME} ${PARAMS_OSX_LIBS})
  endif()
  if((NOT MSVC) AND (DEFINED PARAMS_NOT_WIN_LIBS))
    target_link_libraries(${PARAMS_NAME} ${PARAMS_NOT_WIN_LIBS})
  endif()
  if((NOT LINUX) AND (DEFINED PARAMS_NOT_LINUX_LIBS))
    target_link_libraries(${PARAMS_NAME} ${PARAMS_NOT_LINUX_LIBS})
  endif()
  if((NOT APPLE) AND (DEFINED PARAMS_NOT_OSX_LIBS))
    target_link_libraries(${PARAMS_NAME} ${PARAMS_NOT_OSX_LIBS})
  endif()
endfunction(add_binary)

function(add_static_lib)
  add_binary(TYPE "LIB" LINKAGE "STATIC" ${ARGN})
endfunction()

function(add_lib)
  add_binary(TYPE "LIB" ${ARGN})
endfunction()

function(add_exe)
  add_binary(TYPE "EXE" ${ARGN})
endfunction()

function(add_test_exe)
  add_binary(TYPE "TEST-EXE" ${ARGN})
endfunction()
