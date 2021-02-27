add_library(gas STATIC
  Gasnode.cpp Flowboundarynode.cpp Bernoulligasnode.cpp
  Source.cpp Sink.cpp Innode.cpp
  Gasedge.cpp 
  Shortcomponent.cpp Shortpipe.cpp Controlvalve.cpp Compressorstation.cpp
  Gaspowerconnection.cpp
  Pipe.cpp
  )


target_link_libraries(gas PRIVATE network matrixhandler exception nlohmann_json power coloroutput misc)

target_link_libraries(gas PUBLIC balancelaw scheme mathfunctions equationcomponent)

target_include_directories(gas PUBLIC include)
