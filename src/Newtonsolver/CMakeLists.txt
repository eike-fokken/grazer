add_library(newton INTERFACE)

target_link_libraries(newton INTERFACE matrixhandler exception)

target_include_directories(newton INTERFACE include)
