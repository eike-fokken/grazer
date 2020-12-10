add_library(gas STATIC Gasnode.cpp Gasedge.cpp Innode.cpp  Pipe.cpp  Shortpipe.cpp
  Sink.cpp  Flowboundarynode.cpp Controlvalve.cpp Compressorstation.cpp
  Pressureboundarynode.cpp Gaspowerconnection.cpp)
  

target_link_libraries(gas PRIVATE networkproblem network matrixhandler exception nlohmann_json power coloroutput misc)

target_link_libraries(gas PUBLIC balancelaw scheme mathfunctions)

target_include_directories(gas PUBLIC include)
