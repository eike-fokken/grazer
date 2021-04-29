add_subdirectory(Model)
add_subdirectory(Newtonsolver)
add_subdirectory(Auxillary)
add_subdirectory(validation)


add_library(test_helpers STATIC Equationcomponent_test_helpers.cpp)
target_include_directories(test_helpers PUBLIC include)


