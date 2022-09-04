add_library(balancelaw STATIC
  Isothermaleulerequation.cpp
  Balancelaw_factory.cpp)

target_link_libraries(balancelaw PRIVATE
exception
mathfunctions
aux_unit_conversion
)
target_link_libraries(balancelaw PUBLIC
 nlohmann_json)


target_include_directories(balancelaw PUBLIC
include)
