add_subdirectory(Model)
add_subdirectory(Newtonsolver)
add_subdirectory(Auxillary)


add_library(test_helpers STATIC Equationcomponent_test_helpers.cpp test_io_helper.cpp)

target_link_libraries(test_helpers PUBLIC netfactory networkproblem randutils)
target_link_libraries(test_helpers PUBLIC gtest gtest_main gmock)

target_include_directories(test_helpers PUBLIC include)

