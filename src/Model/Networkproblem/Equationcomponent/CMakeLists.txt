add_library(equationcomponent STATIC Equationcomponent.cpp Valuemap.cpp)

target_link_libraries(equationcomponent PUBLIC mathfunctions exception)

target_include_directories(equationcomponent PUBLIC include)
