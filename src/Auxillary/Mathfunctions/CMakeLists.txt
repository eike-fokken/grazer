add_library(mathfunctions STATIC
  Mathfunctions.cpp
  Stochastic.cpp
  Normaldistribution.cpp
  )

target_link_libraries(mathfunctions PRIVATE exception)

target_include_directories(mathfunctions PUBLIC include)
target_include_directories(mathfunctions SYSTEM PRIVATE system)
# target_include_directories(mathfunctions PUBLIC system)
