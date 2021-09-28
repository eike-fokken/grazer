add_library(componentclasses STATIC Equationcomponent.cpp Valuemap.cpp Statecomponent.cpp)

target_link_libraries(componentclasses PUBLIC mathfunctions exception)

target_include_directories(componentclasses PUBLIC include)
