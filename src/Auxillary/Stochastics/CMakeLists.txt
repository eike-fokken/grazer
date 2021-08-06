
add_library(stochastics STATIC
  Normaldistribution.cpp
  Stochastic.cpp
  )

if (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
  # Suppress warnings because MSVC can't include with SYSTEM
  set(special_debug_msvc_options "/wd4244;/wd4457")
  target_compile_options(stochastics PUBLIC "$<$<CONFIG:DEBUG>:${special_debug_msvc_options}>")
endif()



target_link_libraries(stochastics PRIVATE randutils)
target_link_libraries(stochastics PUBLIC exception)

target_include_directories(stochastics PUBLIC include)



 
