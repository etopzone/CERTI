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
  Federation.cc Federation.hh
  FederationsList.cc FederationsList.hh
  main.cc
  RTIG.cc RTIG.hh
  RTIG_processing.cc
  ${rtig_SRCS_generated}
  )

add_executable(rtig ${rtig_SRCS})
target_link_libraries(rtig CERTI)

if(COMPILE_WITH_CXX11)
    if (CMAKE_VERSION VERSION_LESS 3.1.3)
        include(CheckCXXCompilerFlag)
        check_cxx_compiler_flag(-std=c++14 HAVE_FLAG_STD_CXX14)
        if(HAVE_FLAG_STD_CXX14)
                SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -std=c++14")
        else()
                SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -std=c++1y")
        endif()
    else()
        set_property(TARGET rtig PROPERTY CXX_STANDARD 14)
    endif()
endif()

install(TARGETS rtig
    EXPORT CERTIDepends
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)
