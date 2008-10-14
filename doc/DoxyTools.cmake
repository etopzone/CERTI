# Here is following several macros from
# http://www.cmake.org/Wiki/CMakeMacroListOperations
MACRO(CAR var)
  SET(${var} ${ARGV1})
ENDMACRO(CAR)

MACRO(LIST_CONTAINS var value)
  SET(${var})
  FOREACH (value2 ${ARGN})
    IF (${value} STREQUAL ${value2})
      SET(${var} TRUE)
    ENDIF (${value} STREQUAL ${value2})
  ENDFOREACH (value2)
ENDMACRO(LIST_CONTAINS)

#http://www.cmake.org/Wiki/CMakeMacroParseArguments
MACRO(PARSE_ARGUMENTS prefix arg_names option_names)
  SET(DEFAULT_ARGS)
  FOREACH(arg_name ${arg_names})    
    SET(${prefix}_${arg_name})
  ENDFOREACH(arg_name)
  FOREACH(option ${option_names})
    SET(${prefix}_${option} FALSE)
  ENDFOREACH(option)

  SET(current_arg_name DEFAULT_ARGS)
  SET(current_arg_list)
  FOREACH(arg ${ARGN})    
    LIST_CONTAINS(is_arg_name ${arg} ${arg_names})
    IF (is_arg_name)
      SET(${prefix}_${current_arg_name} ${current_arg_list})
      SET(current_arg_name ${arg})
      SET(current_arg_list)
    ELSE (is_arg_name)
      LIST_CONTAINS(is_option ${arg} ${option_names})
      IF (is_option)
	     SET(${prefix}_${arg} TRUE)
      ELSE (is_option)
	     SET(current_arg_list ${current_arg_list} ${arg})
      ENDIF (is_option)
    ENDIF (is_arg_name)
  ENDFOREACH(arg)
  SET(${prefix}_${current_arg_name} ${current_arg_list})
ENDMACRO(PARSE_ARGUMENTS)

MACRO(DoxyTools_ADD_DocSet)
    PARSE_ARGUMENTS(DoxyTools "CONFIG_FILE;INPUT_FILES;DOC_TYPE;OUTPUT_DIR;COMMENTS" "VERBOSE" ${ARGN})          
    CAR(DOCSET ${DoxyTools_DEFAULT_ARGS})       
    MESSAGE(STATUS "Configuring DOC Set : ${DOCSET}")
    IF (DoxyTools_VERBOSE)     
       MESSAGE(STATUS "    CONFIG_FILE = ${DoxyTools_CONFIG_FILE}")
       MESSAGE(STATUS "    INPUT_FILES = ${DoxyTools_INPUT_FILES}")
       MESSAGE(STATUS "       DOC_TYPE = ${DoxyTools_DOC_TYPE}")
       MESSAGE(STATUS "     OUTPUT_DIR = ${DoxyTools_OUTPUT_DIR}")
       MESSAGE(STATUS "       COMMENTS = ${DoxyTools_COMMENTS}")
    ENDIF(DoxyTools_VERBOSE)
    
    SET(${DOCSET}_DOC_OUTPUT_DIR ${DoxyTools_OUTPUT_DIR} CACHE PATH "${DOCSET} doc output dir")
    # Define appropriate doc type generation for Doxygen
    LIST_CONTAINS(genpdf "pdf" ${DoxyTools_DOC_TYPE}) 
    IF (genpdf)
       SET(${DOCSET}_DOC_GENERATE_LATEX "YES")
    ELSE (genpdf)
       SET(${DOCSET}_DOC_GENERATE_LATEX "NO")
    ENDIF(genpdf)
    
    LIST_CONTAINS(genhtml "html" ${DoxyTools_DOC_TYPE})
    IF (genhtml) 
       SET(${DOCSET}_DOC_GENERATE_HTML "YES")
    ELSE (genhtml)
       SET(${DOCSET}_DOC_GENERATE_HTML "NO")
    ENDIF(genhtml)
    
    # configure Doxygen config file if .in exists
    IF (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${DoxyTools_CONFIG_FILE}.in)
        IF(DoxyTools_VERBOSE)
           MESSAGE(STATUS "Configuring ${DoxyTools_CONFIG_FILE}.in")
        ENDIF(DoxyTools_VERBOSE)
        CONFIGURE_FILE(${DoxyTools_CONFIG_FILE}.in ${CMAKE_CURRENT_BINARY_DIR}/${DoxyTools_CONFIG_FILE})
    ENDIF(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${DoxyTools_CONFIG_FILE}.in)
    
    IF (genhtml) 
       SET(OUTPUT_FILE ${DoxyTools_OUTPUT_DIR}/html/index.html)
       FOREACH(arg ${DoxyTools_INPUT_FILES})
          CONFIGURE_FILE(${arg} ${DoxyTools_OUTPUT_DIR}/html/${arg} COPYONLY)
       ENDFOREACH(arg)   
    ENDIF(genhtml)   
    
    # create output directory
    FILE(MAKE_DIRECTORY ${DoxyTools_OUTPUT_DIR})
    IF(OUTPUT_FILE)
       ADD_CUSTOM_COMMAND(
          OUTPUT  ${OUTPUT_FILE}
          COMMAND ${CMAKE_COMMAND} -E echo_append "Building CERTI ${DOCSET} Documentation..."
          COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/${DoxyTools_CONFIG_FILE}
          COMMAND ${CMAKE_COMMAND} -E echo "Done."
          WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
          DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${DoxyTools_CONFIG_FILE})
       ADD_CUSTOM_TARGET(${DOCSET}_doc ALL DEPENDS ${OUTPUT_FILE})
    ENDIF(OUTPUT_FILE)
      
    ADD_CUSTOM_TARGET(${DOCSET}_doc_forced
           COMMAND ${CMAKE_COMMAND} -E echo_append "Building CERTI ${DOCSET} Documentation..."
           COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/${DoxyTools_CONFIG_FILE}
           COMMAND ${CMAKE_COMMAND} -E echo "Done."
           WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})   
    
    # If genpdf was specified AND a pdflatex compiler was found
    # then create a rule for running pdflatex
    IF(genpdf AND PDFLATEX_COMPILER)
        ADD_CUSTOM_TARGET(${DOCSET}_doc_forced_pdf
           COMMAND ${CMAKE_COMMAND} -E echo_append "Building CERTI ${DOCSET} PDF Documentation..."       
           COMMAND ${PDFLATEX_COMPILER} refman.tex
           COMMAND ${CMAKE_COMMAND} -E echo "Done."
           WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${DOCSET}/latex)                   
    ENDIF(genpdf AND PDFLATEX_COMPILER)
    
    IF (genhtml)
       MESSAGE(STATUS "Will build DOC Set <${DOCSET}> in <${DoxyTools_OUTPUT_DIR}>")
       CONFIGURE_FILE(doc_install.in ${DOCSET}_doc_install.cmake @ONLY)
       INSTALL(SCRIPT ${CMAKE_BINARY_DIR}/${DOCSET}_doc_install.cmake COMPONENT DOC)            
    ENDIF (genhtml)
    
ENDMACRO(DoxyTools_ADD_DocSet)