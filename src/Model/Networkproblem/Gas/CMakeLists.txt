add_library(gas STATIC Gasnode.cpp Gasedge.cpp Innode.cpp  Pipe.cpp  Shortpipe.cpp  Sink.cpp  Gasboundarynode.cpp)
  
target_link_libraries(gas PRIVATE networkproblem network matrixhandler exception nlohmann_json)

target_include_directories(gas PUBLIC include)
