add_library(randutils INTERFACE)

if (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
  # Suppress warnings because MSVC can't include with SYSTEM
  set(special_debug_msvc_options "/wd4244;/wd4457")
  target_compile_options(randutils INTERFACE "$<$<CONFIG:DEBUG>:${special_debug_msvc_options}>")
endif()



target_include_directories(randutils SYSTEM INTERFACE include)
