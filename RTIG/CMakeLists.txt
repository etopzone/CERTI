include_directories(${CMAKE_SOURCE_DIR})

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
    ${CMAKE_CURRENT_SOURCE_DIR}/RTIG_cmdline.c ${CMAKE_CURRENT_SOURCE_DIR}/RTIG_cmdline.h)

if (CMAKE_COMPILER_IS_GNUCC) 
   message(STATUS "INHIBIT Compiler (GCC) warning for gengetopt generated files: ${rtig_SRCS_generated}.")
   set_source_files_properties(${rtig_SRCS_generated}
                               PROPERTIES COMPILE_FLAGS "-w")
endif(CMAKE_COMPILER_IS_GNUCC)

if (MSVC)
   message(STATUS "INHIBIT Compiler (MSVC) warning for gengetopt generated files: ${rtig_SRCS_generated}.")
   set_source_files_properties(${rtig_SRCS_generated}
                               PROPERTIES COMPILE_FLAGS "-D_CRT_SECURE_NO_WARNINGS")
endif(MSVC)

set(rtig_SRCS
  Federate.cc Federate.hh
  Federation.cc Federation_fom.cc Federation.hh
  FederationsList.cc FederationsList.hh
  main.cc
  
  MessageProcessor.cc MessageProcessor.hh
  Mom.cc Mom_interactions.cc Mom_objects.cc Mom.hh
  
  RTIG.cc RTIG.hh
  ${rtig_SRCS_generated}
  )

add_executable(rtig ${rtig_SRCS})
target_link_libraries(rtig CERTI)

install(TARGETS rtig
    EXPORT CERTIDepends
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)
