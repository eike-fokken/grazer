add_custom_target(docs)

if(NOT Doxygen_FOUND)
  message(STATUS "Doxygen not found! building without doxygen docs.")
else()
  set(DOXYGEN_EXTRACT_PRIVATE YES)

  set(DOXYGEN_EXTRACT_ALL YES)
  set(DOXYGEN_BUILTIN_STL_SUPPORT YES)
  set(DOXYGEN_RECURSIVE YES)
  set(DOXYGEN_GENERATE_TREEVIEW YES)

  doxygen_add_docs(doxygen "${PROJECT_SOURCE_DIR}/src" "${PROJECT_SOURCE_DIR}/helper_functions" WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}" )

  # only build latex if doxygen is installed otherwise add_dependencies(docs doxygen) will throw
  add_dependencies(docs doxygen)
endif()

if(NOT LATEX_FOUND)
  message(STATUS "pdflatex not found! building without latex docs.")
else()
  add_latex_document(
    userguide.tex
    TARGET_NAME user_guide
    EXCLUDE_FROM_ALL)
  # explicitly add dependency to doc
  add_dependencies(docs user_guide)
endif()
