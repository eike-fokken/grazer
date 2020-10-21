add_library(exception STATIC Exception.cpp)
target_include_directories(exception PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../../include/Exception")

