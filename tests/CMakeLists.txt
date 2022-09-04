# add_compile_options("-march=native")

# Don't use include(nlohmann_json/CMakeLists.txt) since that carries with it
# unintended consequences that will break the build.  It's generally
# discouraged (although not necessarily well documented as such) to use
# include(...) for pulling in other CMake projects anyways.

include_directories (SYSTEM "${CMAKE_CURRENT_SOURCE_DIR}/../nlohmann_json/single_include/")

# TODO: This should be cleaned up and every target should simply link to
# nlohmann_json, a target that is pulled in as add_subdirectory above before the
# schema validator.




add_subdirectory(Model)
add_subdirectory(Newtonsolver)
add_subdirectory(Auxillary)
add_subdirectory(IpoptWrapper)
add_subdirectory(Optimization_helpers)


add_library(test_helpers STATIC Equationcomponent_test_helpers.cpp test_io_helper.cpp)

target_link_libraries(test_helpers PUBLIC netfactory networkproblem randutils)
target_link_libraries(test_helpers PUBLIC gtest gtest_main gmock)

target_include_directories(test_helpers PUBLIC include)





add_executable(playground_test PlaygroundTest.cpp)
target_link_libraries(playground_test PUBLIC balancelaw scheme)
target_link_libraries(playground_test PUBLIC gtest gtest_main gmock)

add_test(
  NAME playground_test
  COMMAND playground_test
  )
