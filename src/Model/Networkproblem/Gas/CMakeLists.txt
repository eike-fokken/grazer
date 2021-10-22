add_library(gas STATIC
  Gasnode.cpp Flowboundarynode.cpp
  Source.cpp Sink.cpp Innode.cpp
  ConstraintSink.cpp
  Gasedge.cpp 
  Shortcomponent.cpp Shortpipe.cpp Controlvalve.cpp Compressorstation.cpp
  Pipe.cpp
  )


target_link_libraries(gas PRIVATE
matrixhandler
exception
coloroutput
misc
aux_unit_conversion
aux_make_schema)

target_link_libraries(gas PUBLIC
network
balancelaw
scheme
mathfunctions
componentclasses
interpolatingVector)

target_include_directories(gas PUBLIC include)
