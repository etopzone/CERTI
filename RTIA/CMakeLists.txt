
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/libCERTI)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/libRTI)

find_program(GENGETOPT_PROGRAM NAMES gengetopt)
if (GENGETOPT_PROGRAM)
   set(GGOFILE ${CMAKE_CURRENT_SOURCE_DIR}/RTIA.ggo)
   add_custom_target(GenRTIACmdline
                    COMMAND ${GENGETOPT_PROGRAM} --no-handle-error --show-required -i ${GGOFILE}
                    DEPENDS ${GGOFILE}
                    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                    COMMENT "Regenerate RTIA command line C files -- (from ${GGOFILE})"
                    )
endif(GENGETOPT_PROGRAM)
set(rtia_SRCS_generated
    ${CMAKE_CURRENT_SOURCE_DIR}/RTIA_cmdline.c ${CMAKE_CURRENT_SOURCE_DIR}/RTIA_cmdline.h)

IF (CMAKE_COMPILER_IS_GNUCC) 
   message(STATUS "INHIBIT Compiler (GCC) warning for gengetopt generated files: ${rtia_SRCS_generated}.")
   SET_SOURCE_FILES_PROPERTIES(${rtia_SRCS_generated}
                               PROPERTIES COMPILE_FLAGS "-w")
ENDIF(CMAKE_COMPILER_IS_GNUCC)

if (MSVC)
   message(STATUS "INHIBIT Compiler (MSVC) warning for gengetopt generated files: ${rtia_SRCS_generated}.")
   SET_SOURCE_FILES_PROPERTIES(${rtia_SRCS_generated}
                               PROPERTIES COMPILE_FLAGS "-D_CRT_SECURE_NO_WARNINGS")
endif(MSVC)

SET(rtia_SRCS
  Communications.cc Communications.hh
  DataDistribution.cc DataDistribution.hh
  DeclarationManagement.cc DeclarationManagement.hh
  FederationManagement.cc FederationManagement.hh
  Files.cc Files.hh
  main.cc
  ObjectManagement.cc ObjectManagement.hh
  OwnershipManagement.cc OwnershipManagement.hh
  RTIA.cc RTIA.hh
  RTIA_federate.cc
  RTIA_network.cc
  ${rtia_SRCS_generated}
  Statistics.cc Statistics.hh
  TimeManagement.cc TimeManagement.hh  
  )

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/libHLA)

ADD_EXECUTABLE(rtia ${rtia_SRCS})
TARGET_LINK_LIBRARIES(rtia CERTI)
TARGET_LINK_LIBRARIES(rtia HLA)

INSTALL(TARGETS rtia
    EXPORT CERTIDepends
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)
