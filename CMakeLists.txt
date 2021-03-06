cmake_minimum_required (VERSION 3.9)
# 3.9 is needed for CheckIPOSupported and for doxygen_add_docs.

# Version has its own file so that changes can trigger github workflows:
include(Version.cmake)

project(Grazer LANGUAGES C CXX VERSION ${GRAZER_VERSION})
message(STATUS "grazer-version: ${PROJECT_VERSION}")

include(CheckIPOSupported)

#for multiple compiler compatibility:
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

#for development:
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


# for debug builds use the debug configuration of the standard libraries:
# Unfortunately MacOS doesn't ship the debug versions of standard types,
# therefore we forego the debug macro for Apple's Clang.
if(NOT (CMAKE_CXX_COMPILER_ID MATCHES "AppleClang"))
  add_compile_definitions("$<$<CONFIG:DEBUG>:_GLIBCXX_DEBUG;_LIBCPP_DEBUG>")
endif()

option(BUILD_WITH_LIBCPP "Build with libc++ instead of libstdc++, if the compiler is clang." OFF)
# include the googletest project for tests:
# here also the standard library has to be set, if compiling with llvms libc++.
# Our own compile options are set later.
if (BUILD_WITH_LIBCPP AND CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options("-stdlib=libc++")
  add_link_options("-stdlib=libc++")
  message(STATUS "Will compile with libc++.")
endif()
enable_testing()




# https://github.com/google/googletest/blob/master/googletest/README.md#visual-studio-dynamic-vs-static-runtimes
# https://stackoverflow.com/a/12546288/6662425 "how to activate that flag"
if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
  set(gtest_force_shared_crt ON CACHE BOOL "Make gtest compile the C standard libraries as DLLs, because MSVE requires that." FORCE)
endif()
add_subdirectory(googletest)


# Typically you don't care so much for a third party library's tests to be
# run from your own project's code.
set(JSON_BuildTests OFF CACHE INTERNAL "")

# If you only include this third party in PRIVATE source files, you do not
# need to install it when your main project gets installed.
set(JSON_Install OFF CACHE INTERNAL "")
add_subdirectory(nlohmann_json)
if(BUILD_TESTS)
  option(BUILD_TESTS OFF)
  add_subdirectory(json-schema-validator)
  option(BUILD_TESTS ON)
else()
  add_subdirectory(json-schema-validator)
endif()



# An option to exclude IPO from the release type to make better use of a compiler cache (like ccache) for development:
if (NOT DEVEL_RELEASE)

  # Funny enough, this is actually detrimental on my machine.
  # add_compile_options("$<$<CONFIG:RELEASE>:-march=native>")

  # for Release types add link-time optimization
  check_ipo_supported(RESULT is_ipo_supported OUTPUT lto_error)
  if(is_ipo_supported)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ON)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_DEBUG OFF)
    message(STATUS "Interprocedural optimization supported!")
  else()
    message(STATUS "NO INTERPROCESS OPTIMIZATION AVAILABLE!")
    message(STATUS "${lto_error}")
  endif()
else()
  message(STATUS "")
  message(STATUS "Interprocedural deactivated for development purposes!")
  message(STATUS "To reactivate it, try disabling the option DEVEL_RELEASE.")
  message(STATUS "")
endif()
# add_compile_options(-fopenmp)



set(my_debug_options -O0 -Werror -Wconversion -pedantic-errors -pedantic -Wall -Wextra -Wcast-align -Wcast-qual -Wctor-dtor-privacy -Wundef
  -Wdisabled-optimization -Wformat=2 -Winit-self -Wmissing-declarations -Wmissing-include-dirs -Wold-style-cast -Woverloaded-virtual -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-overflow=5 -Wswitch-default -Wunused -Wno-error=unused-parameter -Wno-error=unused -Wno-error=unused-variable -Wno-error=unused-value -Wdelete-non-virtual-dtor -Wno-error=undef)
set(debug_clang_options "-Wunreachable-code;-fcolor-diagnostics;-Wno-error=unused-private-field;-Wmissing-prototypes")

set(debug_gcc_options "-fdiagnostics-color;-Wno-error=strict-overflow;-Wno-error=unused-but-set-variable")

# Warning 4127 warns of if statements that are compile time constants.
# But templated if statements are often compile time constants and Eigen does not have
# if constexpr in those places. Therefore we suppress this warning.
# /Zc:__cplusplus: https://gitlab.com/libeigen/eigen/-/issues/2170 might become superflous
set(debug_msvc_options "/permissive-;/W4;/WX;/wd4127;/wd4100;/Zc:__cplusplus")



set(release_clang_options "")
set(release_gcc_options "")
set(release_msvc_options "/permissive-")



if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options("$<$<CONFIG:DEBUG>:${my_debug_options}>")
  add_compile_options("$<$<CONFIG:DEBUG>:${debug_clang_options}>"
    "$<$<NOT:$<CONFIG:Debug>>:${release_clang_options}>")
elseif (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
  add_compile_options("$<$<CONFIG:DEBUG>:${my_debug_options}>")
   add_compile_options("$<$<CONFIG:DEBUG>:${debug_gcc_options}>"
     "$<$<NOT:$<CONFIG:Debug>>:${release_gcc_options}>")
elseif (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
  add_compile_options(
    "$<$<CONFIG:DEBUG>:${debug_msvc_options}>"
    "$<$<NOT:$<CONFIG:Debug>>:${release_msvc_options}>"
  )
else()
  message(FATAL_ERROR "Unknown compiler: ${CMAKE_CXX_COMPILER_ID}")
endif()



# # for sanitizing in debug mode:
# set(MY_SANITIZE_OPTIONS -fsanitize=address -fno-omit-frame-pointer -fsanitize=undefined)
# add_compile_options("$<$<CONFIG:DEBUG>:${MY_SANITIZE_OPTIONS}>")
# add_link_options("$<$<CONFIG:DEBUG>:${MY_SANITIZE_OPTIONS}>")






## Hacky eigen include:
# find_package (Eigen3 3.3 REQUIRED NO_MODULE)

include_directories (SYSTEM "${CMAKE_CURRENT_SOURCE_DIR}/eigen/")


# Don't use include(nlohmann_json/CMakeLists.txt) since that carries with it
# unintended consequences that will break the build.  It's generally
# discouraged (although not necessarily well documented as such) to use
# include(...) for pulling in other CMake projects anyways.

include_directories (SYSTEM "${CMAKE_CURRENT_SOURCE_DIR}/nlohmann_json/single_include/")


# This includes the pcg random number generator suite.
include_directories (SYSTEM "${CMAKE_CURRENT_SOURCE_DIR}/pcg-cpp/include/")

include_directories(SYSTEM "${CMAKE_CURRENT_SOURCE_DIR}/CLI11/include/")


# for building latex documentation

find_package(Doxygen)
find_package(LATEX COMPONENTS PDFLATEX)
if(LATEX_FOUND)
  include(UseLATEX/UseLATEX.cmake)
endif()

add_subdirectory(docs)


add_subdirectory(src)
add_subdirectory(tests)
add_subdirectory(helper_functions)



add_custom_target(cclean COMMAND cd ${CMAKE_SOURCE_DIR} && rm -r ${CMAKE_SOURCE_DIR}/build/ && mkdir ${CMAKE_SOURCE_DIR}/build/ && cd ${CMAKE_SOURCE_DIR}/build/
  USES_TERMINAL
  )
