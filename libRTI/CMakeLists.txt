
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/libCERTI)

SET(RTI_LIB_SRCS
  RTIambassador.cc
  RTIambPrivateRefs.cc
  RTIambPrivateRefs.hh
  RTItypes.cc
  RTItypesImp.cc
  RTItypesImp.hh
  )
  
SET(RTI_LIB_INCLUDE
   ../include/baseTypes.hh
   ../include/certi.hh
   ../include/federateAmbServices.hh
   ../include/NullFederateAmbassador.hh
   ../include/RTI.hh
   ../include/RTIambServices.hh
   ${CMAKE_BINARY_DIR}/include/RTItypes.hh
)

#SOURCE_GROUP("RTI\\Header" FILES ${RTI_LIB_INCLUDE})

ADD_LIBRARY(RTI ${RTI_LIB_SRCS} ${RTI_LIB_INCLUDE})
TARGET_LINK_LIBRARIES(RTI CERTI FedTime)

IF (BUILD_LEGACY_LIBRTI)
    MESSAGE(STATUS "libRTI variant: CERTI legacy")
    SET_TARGET_PROPERTIES(RTI PROPERTIES OUTPUT_NAME "RTI")
    SET_TARGET_PROPERTIES(RTI PROPERTIES COMPILE_FLAGS "-DLEGACY_LIBRTI")
ELSE (BUILD_LEGACY_LIBRTI)
    MESSAGE(STATUS "libRTI variant: HLA 1.3 NG")
    SET_TARGET_PROPERTIES(RTI PROPERTIES OUTPUT_NAME "RTI-NG")
    SET_TARGET_PROPERTIES(RTI PROPERTIES COMPILE_FLAGS "-DHLA13NG_LIBRTI")
ENDIF (BUILD_LEGACY_LIBRTI)
SET_TARGET_PROPERTIES(RTI PROPERTIES VERSION 1.0.0 SOVERSION 1)
IF (MINGW)
    SET_TARGET_PROPERTIES(RTI PROPERTIES LINK_FLAGS "-Wl,--output-def,${LIBRARY_OUTPUT_PATH}/libRTI.def")
    INSTALL(FILES ${LIBRARY_OUTPUT_PATH}/libRTI.def
            DESTINATION lib)
ENDIF (MINGW)

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

INCLUDE_DIRECTORIES(../libHLA)
SET(FEDTIME_SRCS
    RTIfedTime.cc
    ${CMAKE_SOURCE_DIR}/include/fedtime.hh
)

SOURCE_GROUP("Source Files\\FedTime" FILES ${FEDTIME_SRCS})

ADD_LIBRARY(FedTime
    ${FEDTIME_SRCS}
)

TARGET_LINK_LIBRARIES(FedTime)

SET_TARGET_PROPERTIES(FedTime PROPERTIES PROJECT_LABEL LibFedTime)
IF (MINGW)
    SET_TARGET_PROPERTIES(FedTime PROPERTIES LINK_FLAGS "-Wl,--output-def,${LIBRARY_OUTPUT_PATH}/libFedTime.def")
    INSTALL(FILES ${LIBRARY_OUTPUT_PATH}/libFedTime.def
            DESTINATION lib)
ENDIF (MINGW)

SET_TARGET_PROPERTIES(FedTime PROPERTIES VERSION 1.0.0 SOVERSION 1)

# Install rules for both libRTI-NG and libFedTime
INSTALL(TARGETS RTI FedTime
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)
