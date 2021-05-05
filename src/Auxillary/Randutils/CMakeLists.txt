add_library(randutils INTERFACE)

if (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
  target_compile_options(randutils INTERFACE "$<$<CONFIG:DEBUG>:/wd4244>")
endif()

target_include_directories(randutils SYSTEM INTERFACE include)
