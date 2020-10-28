add_executable(mytest test.cpp)
set_target_properties(mytest PROPERTIES EXCLUDE_FROM_ALL TRUE)
target_link_libraries(mytest PRIVATE exception)
