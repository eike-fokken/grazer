add_library(power STATIC
  Powernode.cpp
  Vphinode.cpp
  PQnode.cpp
  PVnode.cpp
  Transmissionline.cpp
  StochasticPQnode.cpp
  )

target_include_directories(power PUBLIC
  include)

target_link_libraries(power PUBLIC
  componentclasses
  network
  stochastics)
target_link_libraries(power PRIVATE
  matrixhandler
  exception
  aux_make_schema)
