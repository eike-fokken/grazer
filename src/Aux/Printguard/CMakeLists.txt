add_library(printguard STATIC Printguard.cpp)

target_link_libraries(printguard model)
target_include_directories(printguard PUBLIC include)
