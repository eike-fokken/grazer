add_library(printguard STATIC Printguard.cpp)

target_link_libraries(printguard problemlayer)
target_include_directories(printguard PUBLIC include)
