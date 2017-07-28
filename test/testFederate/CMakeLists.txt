include_directories(BEFORE
  ${CMAKE_SOURCE_DIR}/libCERTI 
  ${CMAKE_SOURCE_DIR}/include/)
include_directories(${CERTI_SOURCE_DIR}/libHLA)

# This is used to generate command line option handling
# (using gengetopt)
# the resulting generated file are checked so that you don't
# need the tool if you do not update the command line options
# i.e. there no AUTOMATIC call to this, the regeneration target
#      shall be called explicitely.
find_program(GENGETOPT_PROGRAM NAMES gengetopt)
if (GENGETOPT_PROGRAM)
   set(GGOFILE ${CMAKE_CURRENT_SOURCE_DIR}/testFederate.ggo)
   add_custom_target(GentestFederateCmdline
                    COMMAND ${GENGETOPT_PROGRAM} --no-handle-error --show-required -i ${GGOFILE}
                    DEPENDS ${GGOFILE}
                    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                    COMMENT "Regenerate testFederate command line C files -- (from ${GGOFILE})"
                    )
endif(GENGETOPT_PROGRAM)

set(testFederate_SRCS_generated
    ${CMAKE_CURRENT_SOURCE_DIR}/testFederate_cmdline.c
    ${CMAKE_CURRENT_SOURCE_DIR}/testFederate_cmdline.h)
#avoid this because CMake will remove those file if tagged "GENERATED"
#set_source_files_properties(${testFederate_SRCS_generated} PROPERTIES GENERATED 1)

if(CMAKE_COMPILER_IS_GNUCC) 
   message(STATUS "INHIBIT Compiler (GCC) warning for gengetopt generated files: ${testFederate_SRCS_generated}.")
   set_source_files_properties(${testFederate_SRCS_generated}
                               PROPERTIES COMPILE_FLAGS "-w")
endif(CMAKE_COMPILER_IS_GNUCC)

if (MSVC)
   message(STATUS "INHIBIT Compiler (MSVC) warning for gengetopt generated files: ${testFederate_SRCS_generated}.")
   set_source_files_properties(${testFederate_SRCS_generated}
                               PROPERTIES COMPILE_FLAGS "-D_CRT_SECURE_NO_WARNINGS")
endif(MSVC)

set(testFederate_SRCS
  testFederate.hh
  testFederateMain.cc
  ${testFederate_SRCS_generated}
  )
install(FILES  
  testFederation.fed testFederation.xml
  DESTINATION share/federations)

## HLA 1.3 specific code follows
set(FEDERATE_TARGETNAME "CertiTestFederate-HLA13")
add_executable(${FEDERATE_TARGETNAME} ${testFederate_SRCS} testFederate-HLA13.cc)
set_target_properties(${FEDERATE_TARGETNAME} PROPERTIES COMPILE_FLAGS "-DHLA13")
target_include_directories(${FEDERATE_TARGETNAME} PUBLIC ${CMAKE_SOURCE_DIR}/include/hla-1_3 ${CMAKE_BINARY_DIR}/include/hla-1_3)
target_link_libraries(${FEDERATE_TARGETNAME} RTI)
install(TARGETS ${FEDERATE_TARGETNAME}
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)

## IEEE1516_2000 specific code follow
set(FEDERATE_TARGETNAME "CertiTestFederate-IEEE1516_2000")
add_executable(${FEDERATE_TARGETNAME} ${testFederate_SRCS} testFederate-IEEE1516_2000.cc)
set_target_properties(${FEDERATE_TARGETNAME} PROPERTIES COMPILE_FLAGS "-DIEEE1516_2000")
target_include_directories(${FEDERATE_TARGETNAME} PUBLIC ${CMAKE_SOURCE_DIR}/include/ieee1516-2000 ${CMAKE_BINARY_DIR}/include/ieee1516-2000)
target_link_libraries(${FEDERATE_TARGETNAME} RTI1516)
install(TARGETS ${FEDERATE_TARGETNAME}
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)

## IEEE1516_2010 specific code follow
set(FEDERATE_TARGETNAME "CertiTestFederate-IEEE1516_2010")
add_executable(${FEDERATE_TARGETNAME} ${testFederate_SRCS} testFederate-IEEE1516_2010.cc)
set_target_properties(${FEDERATE_TARGETNAME} PROPERTIES COMPILE_FLAGS "-DIEEE1516_2010")
target_include_directories(${FEDERATE_TARGETNAME} PUBLIC ${CMAKE_SOURCE_DIR}/include/ieee1516-2010 ${CMAKE_BINARY_DIR}/include/ieee1516-2010)
target_link_libraries(${FEDERATE_TARGETNAME} RTI1516e)
install(TARGETS ${FEDERATE_TARGETNAME}
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)

# DTest script not ready [yet]
install(FILES  
        dtest_testFederate.py 
        PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE WORLD_READ WORLD_EXECUTE
        DESTINATION share/scripts)

