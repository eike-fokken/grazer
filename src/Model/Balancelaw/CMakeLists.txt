add_library(balancelaw STATIC
  Pipe_Balancelaw.cpp
  Isothermaleulerequation.cpp
  Balancelaw_factory.cpp)

target_link_libraries(balancelaw PRIVATE
exception
mathfunctions)

target_include_directories(balancelaw PUBLIC
include)
