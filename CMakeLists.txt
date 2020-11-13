cmake_minimum_required (VERSION 3.12...3.18)

project(Grazer)
include(CheckIPOSupported)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

enable_testing()
add_subdirectory(googletest)

#for multiple compiler compatibility:
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# for Release types add link-time optimization
# for Debug types add warnings instead!
check_ipo_supported(RESULT ipo_is_supported)
if(CMAKE_BUILD_TYPE)
  if(${CMAKE_BUILD_TYPE} MATCHES "Release" OR ${CMAKE_BUILD_TYPE} MATCHES "Relwithdebinfo")
    if(ipo_is_supported)
      set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
    endif()
  else()
    add_compile_options(
      -Og -Werror -Wconversion -pedantic-errors -pedantic -Wall -Wextra -Wcast-align -Wcast-qual -Wctor-dtor-privacy -Wdisabled-optimization -Wformat=2 -Winit-self -Wmissing-declarations -Wmissing-include-dirs  -Wold-style-cast -Woverloaded-virtual -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo  -Wstrict-overflow=5 -Wswitch-default -Wundef -Wunused -Wno-error=unused-parameter -Wno-error=unused-private-field -Wno-error=unused-variable
      # -Wall -Wextra -pedantic -Werror
      )
  endif()
else()# empty build type
  if(ipo_is_supported)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
  endif()
endif()




find_package (Eigen3 3.3 REQUIRED NO_MODULE)
if (NOT EIGEN3_FOUND)
  if(NOT EIGEN3_INCLUDE_DIR)
    message(STATUS " Eigen Include dir: ${EIGEN_INCLUDE_DIR}")
    message(FATAL_ERROR "Eigen was not found and no include directory was given. Please provide EIGEN3_INCLUDE_DIR with the commandline option
-DEIGEN_INCLUDE_DIR='path/to/eigen'")
  else()
    include_directories (SYSTEM ${EIGEN3_INCLUDE_DIR})
  endif()
endif()



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


add_custom_target(cclean COMMAND cd ${CMAKE_SOURCE_DIR} && rm -r ${CMAKE_SOURCE_DIR}/build/ && mkdir ${CMAKE_SOURCE_DIR}/build/ && cd ${CMAKE_SOURCE_DIR}/build/
  USES_TERMINAL
  )
