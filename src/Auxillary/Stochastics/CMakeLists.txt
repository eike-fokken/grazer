
add_library(stochastics STATIC
  Normaldistribution.cpp
  )

if (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
  target_compile_options(stochastics PUBLIC "$<$<CONFIG:DEBUG>:/wd4244>")
endif()



target_link_libraries(stochastics PRIVATE randutils)
target_link_libraries(stochastics PUBLIC exception)

target_include_directories(stochastics PUBLIC include)



 
