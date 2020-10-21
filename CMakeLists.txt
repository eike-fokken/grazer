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
      -pedantic -Wall -Wextra -Wcast-align -Wcast-qual -Wctor-dtor-privacy -Wdisabled-optimization -Wformat=2 -Winit-self -Wmissing-declarations -Wmissing-include-dirs  -Wold-style-cast -Woverloaded-virtual -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo  -Wstrict-overflow=5 -Wswitch-default -Wundef -Wunused -Werror
      # -Wall -Wextra -pedantic -Werror
      )
  endif()
else()# empty build type
  if(ipo_is_supported)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
  endif()
endif()

find_package(Doxygen)
if(NOT Doxygen_FOUND)
  message(STATUS "Doxygen not found! building without docs")
else()
  add_subdirectory(docs)
  ## build the docs, if type is release
  if(${CMAKE_BUILD_TYPE} MATCHES "Release")
    add_custom_target(release ALL)
    add_dependencies(release docs)
  endif()
endif()

add_subdirectory(src)
add_subdirectory(tests)
add_subdirectory(testingWithGoogle)

add_custom_target(cclean COMMAND cd ${CMAKE_SOURCE_DIR} && rm -r ${CMAKE_SOURCE_DIR}/build/ && mkdir ${CMAKE_SOURCE_DIR}/build/ && cd ${CMAKE_SOURCE_DIR}/build/
  USES_TERMINAL
  )
