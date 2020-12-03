add_library(balancelaw STATIC Isothermaleulerequation.cpp)

target_link_libraries(balancelaw PRIVATE exception mathfunctions)

target_include_directories(balancelaw PUBLIC include)
