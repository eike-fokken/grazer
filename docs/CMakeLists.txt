set(DOXYGEN_EXTRACT_PRIVATE YES)

set(DOXYGEN_EXTRACT_ALL YES)
set(DOXYGEN_BUILTIN_STL_SUPPORT YES)
set(DOXYGEN_RECURSIVE YES)
set(DOXYGEN_GENERATE_TREEVIEW YES)


add_custom_target(docs)

doxygen_add_docs(doxygen "${PROJECT_SOURCE_DIR}/src" "${PROJECT_SOURCE_DIR}/helper_functions" WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}" )

add_latex_document(
  main.tex
  TARGET_NAME user_guide
  EXCLUDE_FROM_ALL)
# explicitly add dependency to doc
add_dependencies(docs user_guide doxygen)
