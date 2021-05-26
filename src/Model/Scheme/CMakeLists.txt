add_library(scheme STATIC
  Implicitboxscheme.cpp
  Scheme_factory.cpp)

target_link_libraries(scheme PRIVATE
  balancelaw
  exception)

target_include_directories(scheme PUBLIC
  include)
