include_directories(BEFORE
  ${CMAKE_SOURCE_DIR}/libCERTI 
  ${CMAKE_SOURCE_DIR}/test/libgraphc
  ${CMAKE_SOURCE_DIR}/include/
  ${CMAKE_SOURCE_DIR}/include/hla-1_3
  ${CMAKE_BINARY_DIR}/include/hla-1_3)
include_directories(${CERTI_SOURCE_DIR}/libHLA)

if (WIN32)
   add_definitions(-DHAVE_STRING_H)
endif (WIN32)

find_program(GENGETOPT_PROGRAM NAMES gengetopt)
if (GENGETOPT_PROGRAM)
   set(GGOFILE ${CMAKE_CURRENT_SOURCE_DIR}/Billard.ggo)
   add_custom_target(GenBillardCmdline
                    COMMAND ${GENGETOPT_PROGRAM} --no-handle-error --show-required -i ${GGOFILE}
                    DEPENDS ${GGOFILE}
                    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                    COMMENT "Regenerate Billard command line C files -- (from ${GGOFILE})"
                    )
endif(GENGETOPT_PROGRAM)
set(Billard_SRCS_generated
    Billard_cmdline.c Billard_cmdline.h)

if (CMAKE_COMPILER_IS_GNUCC) 
   message(STATUS "INHIBIT Compiler (GCC) warning for gengetopt generated files: ${Billard_SRCS_generated}.")
   set_source_files_properties(${Billard_SRCS_generated}
                               PROPERTIES COMPILE_FLAGS "-w")
endif(CMAKE_COMPILER_IS_GNUCC)

if (MSVC)
   message(STATUS "INHIBIT Compiler (MSVC) warning for gengetopt generated files: ${Billard_SRCS_generated}.")
   set_source_files_properties(${rtig_SRCS_generated}
                               PROPERTIES COMPILE_FLAGS "-D_CRT_SECURE_NO_WARNINGS")
endif(MSVC)

########### next target ###############

set(billard_SRCS
  Billard.cc
  BillardDDM.cc
  Ball.cc
  ColoredBall.cc
  Display.cc
  main.cc
  cmdline.c
  getopt.c
  getopt1.c
  #${Billard_SRCS_generated} not ready [yet]
  )
  
if(X_DISPLAY_MISSING)
  message(STATUS "Some X11 header were not found: only Billard Test Application with no GUI will be built.")
else(X_DISPLAY_MISSING)
  add_executable(billard ${billard_SRCS})
  target_link_libraries(billard graph RTI FedTime)  

  install(TARGETS billard
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)  
endif(X_DISPLAY_MISSING)

add_executable(billard-nogui ${billard_SRCS})
set_target_properties(billard-nogui PROPERTIES COMPILE_FLAGS "-DX_DISPLAY_MISSING=1")
target_link_libraries(billard-nogui graph-nogui RTI FedTime)

install(TARGETS billard-nogui
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)  
    
########### install files ###############
install(FILES
  Test.fed Test.xml 
  DESTINATION share/federations)
  
install(FILES
  dtest_billard.py 
  PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE WORLD_READ WORLD_EXECUTE
  DESTINATION share/scripts)

