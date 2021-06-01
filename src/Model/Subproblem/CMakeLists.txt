add_library(subproblem STATIC Subproblem.cpp )

target_link_libraries(subproblem PRIVATE exception)

target_include_directories(subproblem PUBLIC include)
