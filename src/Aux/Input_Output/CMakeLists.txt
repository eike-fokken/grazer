add_library(input_output STATIC Input_output.cpp)
target_include_directories(input_output PUBLIC "include")
target_link_libraries(input_output PRIVATE exception)
