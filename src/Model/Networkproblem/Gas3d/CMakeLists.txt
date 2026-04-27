add_library(gas3d STATIC
  Gas3dedge.cpp
  Gas3dnode.cpp
  Flowboundarynode.cpp

  Source.cpp
  Sink.cpp
  Innode.cpp

  Pipe.cpp)

target_link_libraries(gas3d PRIVATE
matrixhandler
exception
coloroutput
misc
aux_unit_conversion
aux_make_schema)

target_link_libraries(gas3d PUBLIC
network
balancelaw
scheme
mathfunctions
componentclasses
interpolatingVector)

target_include_directories(gas3d PUBLIC include)
