add_library(aux_executable STATIC Aux_executable.cpp)
target_include_directories(aux_executable PUBLIC "include")
target_link_libraries(aux_executable PRIVATE exception)
