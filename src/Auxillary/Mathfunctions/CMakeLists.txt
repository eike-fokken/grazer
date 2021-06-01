add_library(mathfunctions STATIC
  Mathfunctions.cpp
  )

target_link_libraries(mathfunctions PRIVATE exception)

target_include_directories(mathfunctions PUBLIC include)


