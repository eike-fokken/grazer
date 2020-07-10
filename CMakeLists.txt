cmake_minimum_required (VERSION 3.12...3.18)

project(NetworkClass)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#for multiple compiler compatibility:
set(CMAKE_CXX_EXTENSIONS OFF)



# for Release types add link-time optimization
# for Debug types add warnings instead!
if(DEFINED ENV{CMAKE_BUILD_TYPE})
  if(${CMAKE_BUILD_TYPE} MATCHES "Release" OR ${CMAKE_BUILD_TYPE} MATCHES "Relwithdebinfo")
    if( ipo_supported )
      set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
    endif()
  else()
    add_compile_options(
      -pedantic -Wall -Wextra -Wcast-align -Wcast-qual -Wctor-dtor-privacy -Wdisabled-optimization -Wformat=2 -Winit-self -Wmissing-declarations -Wmissing-include-dirs  -Wold-style-cast -Woverloaded-virtual -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo  -Wstrict-overflow=5 -Wswitch-default -Wundef -Wunused -Werror 
      # -Wall -Wextra -pedantic -Werror
      )
  endif()
else()# empty build type
  if( ipo_supported )
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
  endif()
endif()



add_subdirectory(src)
add_subdirectory(tests)
