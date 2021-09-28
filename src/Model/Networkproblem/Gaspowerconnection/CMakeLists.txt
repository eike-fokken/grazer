add_library(gaspowerconnection STATIC
  Gaspowerconnection.cpp
  ExternalPowerplant.cpp
  )

target_link_libraries(gaspowerconnection PRIVATE
  network
  matrixhandler
  exception
  power
  aux_unit_conversion
  misc
  aux_make_schema)

target_link_libraries(gaspowerconnection PUBLIC
  mathfunctions
  componentclasses
  gas)

target_include_directories(gaspowerconnection PUBLIC
  include)
