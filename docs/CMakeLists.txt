if(${CMAKE_BUILD_TYPE} MATCHES "Release")
  add_custom_target(release ALL)
  add_dependencies(release docs)
elseif(${CMAKE_BUILD_TYPE} MATCHES "Relwithdebinfo" OR ${CMAKE_BUILD_TYPE} MATCHES "Debug")
  add_custom_target(debug ALL)
  add_dependencies(debug docs)
  set(DOXYGEN_EXTRACT_PRIVATE YES)
endif()



set(DOXYGEN_EXTRACT_ALL YES)
set(DOXYGEN_BUILTIN_STL_SUPPORT YES)
set(DOXYGEN_RECURSIVE YES)


doxygen_add_docs(docs WORKING_DIRECTORY  "${PROJECT_SOURCE_DIR}/src")

