add_library(balancelaw STATIC
  Isothermaleulerequation.cpp
  )

target_link_libraries(balancelaw PRIVATE
exception
mathfunctions
aux_unit_conversion
)

target_include_directories(balancelaw PUBLIC
include)
