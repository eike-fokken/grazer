
add_library(stochastics STATIC
  Normaldistribution.cpp
  Stochastic.cpp
  )

target_link_libraries(stochastics PRIVATE randutils)
target_link_libraries(stochastics PUBLIC exception)

target_include_directories(stochastics PUBLIC include)



 
