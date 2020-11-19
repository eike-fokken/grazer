add_library(newton INTERFACE )

target_link_libraries(newton INTERFACE matrixhandler problemlayer)

target_include_directories(newton INTERFACE include)
