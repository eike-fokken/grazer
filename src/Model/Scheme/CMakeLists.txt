add_library(scheme INTERFACE
  # Implicitboxscheme.cpp
  # Scheme_factory.cpp
)

target_link_libraries(scheme INTERFACE
  balancelaw
  exception)

target_include_directories(scheme INTERFACE
  include)
