add_library(ipoptwrapper STATIC Wrapper.cpp)
target_link_libraries(ipoptwrapper PUBLIC my_ipopt)
target_include_directories(ipoptwrapper PUBLIC include)
