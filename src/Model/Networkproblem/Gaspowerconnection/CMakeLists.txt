add_library(gaspowerconnection STATIC  Gaspowerconnection.cpp)

target_link_libraries(gaspowerconnection PRIVATE network matrixhandler exception  power aux_unit_conversion misc)

target_link_libraries(gaspowerconnection PUBLIC mathfunctions equationcomponent gas)

target_include_directories(gaspowerconnection PUBLIC include)
