IF (BUILD_DOC)
    FIND_PACKAGE(Doxygen)
    FIND_PACKAGE(LATEX)
    IF (DOXYGEN_FOUND)
    LIST(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}")
    INCLUDE(DoxyTools)
    IF (WIN32)
      SET(DOXYGEN_GENERATE_MAN NO)
      FIND_PROGRAM(HHC_PROGRAM
        NAMES hhc.exe
        PATHS "c:/Program Files/HTML Help Workshop"
        DOC "HTML Help Compiler program")
    ELSE (WIN32)
      SET(DOXYGEN_GENERATE_MAN YES)
      SET(HHC_PROGRAM)
    ENDIF (WIN32)
    OPTION(BUILD_DOC_INSTALL
             "Build CERTI installation documentation" ON)
    OPTION(BUILD_DOC_USER
             "Build CERTI user documentation" ON)
    OPTION(BUILD_DOC_DEVELOPER
             "Build CERTI developer documentation" OFF)
    ENDIF(DOXYGEN_FOUND)
ENDIF(BUILD_DOC)

IF (DOXYGEN_FOUND)
  #install documentation
  IF (BUILD_DOC_INSTALL)
      DoxyTools_ADD_DocSet(Install
                           CONFIG_FILE certi_install.cnf
                           INPUT_FILES certi_install_header.html certi_footer.html
                           DOC_TYPE    "html;pdf"
                           OUTPUT_DIR ${CMAKE_BINARY_DIR}/doc/Install
                           COMMENTS "CERTI installation documentation"
                           )
  ENDIF (BUILD_DOC_INSTALL)

  #user documentation
  IF (BUILD_DOC_USER)
      DoxyTools_ADD_DocSet(User
                           CONFIG_FILE certi_user.cnf
                           INPUT_FILES certi_header.html certi_footer.html
                           DOC_TYPE    "html;pdf"
                           OUTPUT_DIR ${CMAKE_BINARY_DIR}/doc/User
                           COMMENTS "CERTI user documentation"
                           )
  ENDIF (BUILD_DOC_USER)

  #developer documentation
  IF (BUILD_DOC_DEVELOPER)
      DoxyTools_ADD_DocSet(Developer
                           CONFIG_FILE certi_developer.cnf
                           INPUT_FILES certi_header.html certi_footer.html
                           DOC_TYPE    "html"
                           OUTPUT_DIR ${CMAKE_BINARY_DIR}/doc/Developer
                           COMMENTS "CERTI developer documentation"
                           )
  ENDIF (BUILD_DOC_DEVELOPER)
ENDIF (DOXYGEN_FOUND)

configure_file(index.html ${CMAKE_BINARY_DIR}/doc COPYONLY)
configure_file(favicon.ico ${CMAKE_BINARY_DIR}/doc COPYONLY)
configure_file(certi.css ${CMAKE_BINARY_DIR}/doc COPYONLY)

INSTALL(FILES
    index.html favicon.ico
    DESTINATION share/doc/certi)
