add_library(input_output STATIC Input_output.cpp)

if (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
  # Suppress warnings because MSVC can't include with SYSTEM
  set(special_debug_msvc_options "/wd4996")
  target_compile_options(input_output PUBLIC "$<$<CONFIG:DEBUG>:${special_debug_msvc_options}>")
endif()

target_include_directories(input_output PUBLIC "include")
target_link_libraries(input_output PRIVATE exception)
