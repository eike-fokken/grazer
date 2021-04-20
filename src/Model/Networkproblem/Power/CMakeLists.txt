add_library(power STATIC
  Powernode.cpp
  Vphinode.cpp
  PQnode.cpp
  PVnode.cpp
  Transmissionline.cpp
  StochasticPQnode.cpp
  SwitchedPowerplant.cpp)

target_include_directories(power PUBLIC
  include)

target_link_libraries(power PUBLIC
  equationcomponent
  network
  stochastics)
target_link_libraries(power PRIVATE
  matrixhandler
  exception
  aux_make_schema)
