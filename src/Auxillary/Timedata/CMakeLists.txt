add_library(timedata STATIC Timedata.cpp)
target_link_libraries(timedata PUBLIC
  nlohmann_json)

target_include_directories(timedata PUBLIC include)
