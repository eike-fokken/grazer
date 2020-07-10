
# set(INCLUDE_VAR "${CMAKE_SOURCE_DIR}/include/Network")
# set(NETWORK_HEADERS ${INCLUDE_VAR}/Net.hpp ${INCLUDE_VAR}/Node.hpp ${INCLUDE_VAR}/Edge.hpp )

add_library(network STATIC Net.cpp Edge.cpp Node.cpp ${NETWORK_HEADERS})
