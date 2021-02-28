cmake_minimum_required (VERSION 3.12...3.18)

project(Grazer C CXX)
include(CheckIPOSupported)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(BUILD_WITH_LIBCPP "Build with libc++ instead of libstdc++, if the compiler is clang." OFF)

# include the googletest project for tests:
# here also the standard library has to be set, if compiling with llvms libc++.
# Our own compile options are set later.
if (BUILD_WITH_LIBCPP AND CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  add_compile_options("-stdlib=libc++")
  add_link_options("-stdlib=libc++")
  message(STATUS "Will compile with libc++.")
endif()
enable_testing()
add_subdirectory(googletest)

#for multiple compiler compatibility:
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


#set(CMAKE_POSITION_INDEPENDENT_CODE ON)
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

# add_compile_options(-fopenmp)



set(my_debug_options -O0 -Werror -Wconversion -pedantic-errors -pedantic -Wall -Wextra -Wcast-align -Wcast-qual -Wctor-dtor-privacy -Wdisabled-optimization -Wformat=2 -Winit-self -Wmissing-declarations -Wmissing-include-dirs -Wold-style-cast -Woverloaded-virtual -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-overflow=5 -Wswitch-default -Wundef -Wunused -Wno-error=unused-parameter -Wno-error=unused-variable -Wno-error=unused-value -Wdelete-non-virtual-dtor)
set(debug_clang_options "-fcolor-diagnostics;-Wno-error=unused-private-field")

set(debug_gcc_options "-fdiagnostics-color;-Wno-error=strict-overflow")

# Warning 4127 warns of if statements that are compile time constants.
# But templated if statements are often compile time constants and Eigen does not have
# if constexpr in those places. Therefore we suppress this warning.
# W3 instead of W4: eigen uses deprecated features 
# (Eigen\src/Core/util/Meta.h(568,2): warning C4996: 'std::equal_to<double>::result_type': warning STL4007: Many result_type typedefs and all argument_type, first_argument_type, and second_argument_type typedefs are deprecated in C++17.)
set(debug_msvc_options "/permissive-;/W3;/WX;/wd4127")



set(release_clang_options "")
set(release_gcc_options "")
set(release_msvc_options "/permissive-")



if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  add_compile_options("$<$<CONFIG:DEBUG>:${my_debug_options}>")
  add_compile_options("$<$<CONFIG:DEBUG>:${debug_clang_options}>"
    "$<$<NOT:$<CONFIG:Debug>>:${release_clang_options}>")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  add_compile_options("$<$<CONFIG:DEBUG>:${my_debug_options}>")
   add_compile_options("$<$<CONFIG:DEBUG>:${debug_gcc_options}>"
     "$<$<NOT:$<CONFIG:Debug>>:${release_gcc_options}>")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  add_compile_options(
    "$<$<CONFIG:DEBUG>:${debug_msvc_options}>"
    "$<$<NOT:$<CONFIG:Debug>>:${release_msvc_options}>"
  )
else()
  message(FATAL "Unknown compiler: ${CMAKE_CXX_COMPILER_ID}")
endif()


# add_compile_options("$<$<CONFIG:RELEASE>:-march=native>"
#     )

# # for sanitizing in debug mode:
# set(MY_SANITIZE_OPTIONS -fsanitize=address -fno-omit-frame-pointer -fsanitize=undefined)
# add_compile_options("$<$<CONFIG:DEBUG>:${MY_SANITIZE_OPTIONS}>")
# add_link_options("$<$<CONFIG:DEBUG>:${MY_SANITIZE_OPTIONS}>")






## Hacky eigen include:
# find_package (Eigen3 3.3 REQUIRED NO_MODULE)

include_directories (SYSTEM "${CMAKE_CURRENT_SOURCE_DIR}/eigen/")




find_package(Doxygen)
if(NOT Doxygen_FOUND)
  message(STATUS "Doxygen not found! building without docs")
else()
  add_subdirectory(docs)
endif()

add_subdirectory(src)
add_subdirectory(testingWithGoogle)


# Typically you don't care so much for a third party library's tests to be
# run from your own project's code.
set(JSON_BuildTests OFF CACHE INTERNAL "")

# If you only include this third party in PRIVATE source files, you do not
# need to install it when your main project gets installed.
set(JSON_Install OFF CACHE INTERNAL "")

# Don't use include(nlohmann_json/CMakeLists.txt) since that carries with it
# unintended consequences that will break the build.  It's generally
# discouraged (although not necessarily well documented as such) to use
# include(...) for pulling in other CMake projects anyways.
add_subdirectory(nlohmann_json)

add_subdirectory(helper_functions)

add_custom_target(cclean COMMAND cd ${CMAKE_SOURCE_DIR} && rm -r ${CMAKE_SOURCE_DIR}/build/ && mkdir ${CMAKE_SOURCE_DIR}/build/ && cd ${CMAKE_SOURCE_DIR}/build/
  USES_TERMINAL
  )
