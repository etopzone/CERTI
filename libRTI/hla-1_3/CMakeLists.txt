# Add standard specific include directory
include_directories(${CMAKE_SOURCE_DIR}/include/hla-1_3)
include_directories(${CMAKE_BINARY_DIR}/include/hla-1_3)

set(RTI_LIB_SRCS
    RTIambassador.cc
    RTIambPrivateRefs.cc
    RTIambPrivateRefs.hh
    RTItypes.cc
    RTItypesImp.cc
    RTItypesImp.hh
   )

set(RTI_LIB_INCLUDEENDIF
    ${CMAKE_SOURCE_DIR}/include/hla-1_3/baseTypes.hh
    ${CMAKE_SOURCE_DIR}/include/certi.hh
    ${CMAKE_SOURCE_DIR}/include/hla-1_3/federateAmbServices.hh
    ${CMAKE_SOURCE_DIR}/include/hla-1_3/NullFederateAmbassador.hh
    ${CMAKE_SOURCE_DIR}/include/hla-1_3/RTI.hh
    ${CMAKE_SOURCE_DIR}/include/hla-1_3/RTIambServices.hh
    ${CMAKE_BINARY_DIR}/include/hla-1_3/RTItypes.hh
   )

add_library(RTI ${RTI_LIB_SRCS} ${RTI_LIB_INCLUDE})
target_link_libraries(RTI CERTI FedTime)

if (BUILD_LEGACY_LIBRTI)
    message(STATUS "libRTI variant: CERTI legacy")
    set_target_properties(RTI PROPERTIES OUTPUT_NAME "RTI")
    set_target_properties(RTI PROPERTIES COMPILE_FLAGS "-DLEGACY_LIBRTI")
else (BUILD_LEGACY_LIBRTI)
    message(STATUS "libRTI variant: HLA 1.3 NG")
    if (WIN32 AND CERTI_RTING_DLL_USE_LIB_PREFIX)
       set_target_properties(RTI PROPERTIES OUTPUT_NAME "libRTI-NG")
    else()
       set_target_properties(RTI PROPERTIES OUTPUT_NAME "RTI-NG")
    endif()
    set_target_properties(RTI PROPERTIES COMPILE_FLAGS "-DHLA13NG_LIBRTI")
endif (BUILD_LEGACY_LIBRTI)
set_target_properties(RTI PROPERTIES VERSION 1.0.0 SOVERSION 1)
if (MINGW)
    set_target_properties(RTI PROPERTIES LINK_FLAGS "-Wl,--output-def,${LIBRARY_OUTPUT_PATH}/libRTI.def")
    install(FILES ${LIBRARY_OUTPUT_PATH}/libRTI.def
            DESTINATION lib)
endif (MINGW)


##################################################################
# Lib FedTime
# A replacement lib FedTime may be provided by the user
# CERTI is provided one. The lib FedTime depends on libRTI
# since it inherits from the RTI::FedTime abstract class.
##################################################################
IF(CMAKE_COMPILER_IS_GNUCC)
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-long-long")
ENDIF(CMAKE_COMPILER_IS_GNUCC)
IF(CMAKE_COMPILER_IS_GNUCXX)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-long-long")
ENDIF(CMAKE_COMPILER_IS_GNUCXX)

include_directories(${CMAKE_SOURCE_DIR}/libHLA)
set(FEDTIME_SRCS
    RTIfedTime.cc
    ${CMAKE_SOURCE_DIR}/include/hla-1_3/fedtime.hh
)

source_group("Source Files\\FedTime" FILES ${FEDTIME_SRCS})

add_library(FedTime ${FEDTIME_SRCS})
# Correct line
target_link_libraries(FedTime)
# Incorrect line
# (at least the dependency exists but we cannot specify that
#  because we would generate a circular deps RTI-->FedTime-->RTI)
#TARGET_LINK_LIBRARIES(FedTime RTI)

IF (WIN32 AND CERTI_RTING_DLL_USE_LIB_PREFIX)
   set_target_properties(FedTime PROPERTIES OUTPUT_NAME "libFedTime")
ENDIF()

set_target_properties(FedTime PROPERTIES PROJECT_LABEL LibFedTime)
IF (MINGW)
    set_target_properties(FedTime PROPERTIES LINK_FLAGS "-Wl,--output-def,${LIBRARY_OUTPUT_PATH}/libFedTime.def")
    INSTALL(FILES ${LIBRARY_OUTPUT_PATH}/libFedTime.def
            DESTINATION lib)
ENDIF (MINGW)

set_target_properties(FedTime PROPERTIES VERSION 1.0.0 SOVERSION 1)

set(TestFedTime_SRCS
  TestFedTime.cc
)
add_executable(TestFedTime ${TestFedTime_SRCS})
target_link_libraries(TestFedTime FedTime RTI)
add_test(NAME TestLibFedTime COMMAND $<TARGET_FILE:TestFedTime>)

# Install rules for both libRTI-NG and libFedTime
install(TARGETS RTI FedTime
        EXPORT CERTIDepends
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)
