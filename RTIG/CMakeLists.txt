include_directories(${CERTI_SOURCE_DIR}/libHLA)
include_directories(${CMAKE_SOURCE_DIR}/libCERTI)
include_directories(${CMAKE_SOURCE_DIR}/libRTI)

find_program(GENGETOPT_PROGRAM NAMES gengetopt)
if (GENGETOPT_PROGRAM)
   set(GGOFILE ${CMAKE_CURRENT_SOURCE_DIR}/RTIG.ggo)
   add_custom_target(GenRTIGCmdline
                    COMMAND ${GENGETOPT_PROGRAM} --no-handle-error --show-required -i ${GGOFILE}
                    DEPENDS ${GGOFILE}
                    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                    COMMENT "Regenerate RTIG command line C files -- (from ${GGOFILE})"
                    )
endif(GENGETOPT_PROGRAM)
set(rtig_SRCS_generated
    RTIG_cmdline.c RTIG_cmdline.h)

IF (CMAKE_COMPILER_IS_GNUCC) 
   message(STATUS "INHIBIT Compiler warning for gengetopt generated files")
   SET_SOURCE_FILES_PROPERTIES(${rtig_SRCS_generated}
                               PROPERTIES COMPILE_FLAGS "-w")
ENDIF(CMAKE_COMPILER_IS_GNUCC)

set(rtig_SRCS
  Federate.cc Federate.hh
  Federation.cc Federation.hh
  FederationsList.cc FederationsList.hh
  main.cc
  RTIG.cc RTIG.hh
  RTIG_processing.cc
  Trace.cc Trace.hh
  ${rtig_SRCS_generated}
  )

add_executable(rtig ${rtig_SRCS})
target_link_libraries(rtig CERTI)

install(TARGETS rtig
    EXPORT CERTIDepends
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)
