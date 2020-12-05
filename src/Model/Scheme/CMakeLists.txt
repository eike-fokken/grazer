add_library(scheme STATIC Implicitboxscheme.cpp)

target_link_libraries(scheme PRIVATE balancelaw)

target_include_directories(scheme PUBLIC include)
