INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/libCERTI)

SET(RTI1516_LIB_SRCS
  RTI1516ambassadorFactory.cpp
  RTI1516exception.cpp
  RTI1516variableLengthData.cpp
  RTI1516variableLengthDataImplementation.h
  RTI1516ambassador.cpp
  RTI1516ambassador.h
  RTI1516ambPrivateRefs.cpp
  RTI1516ambPrivateRefs.h
  RTI1516Handle.cpp
  RTI1516HandleImplementation.cpp
  RTI1516HandleImplementation.h
  RTI1516ambTime.cpp
  RTI1516fedAmbassador.cpp
  RTI1516HandleFactory.cpp
  RTI1516HandleFactory.h
  )

SET(RTI1516_LIB_INCLUDE
   ${CMAKE_BINARY_DIR}/include/RTItypes.hh
   ../include/RTI/Enums.h
   ../include/RTI/Exception.h
   ../include/RTI/FederateAmbassador.h
   ../include/RTI/Handle.h
   ../include/RTI/LogicalTime.h
   ../include/RTI/LogicalTimeFactory.h
   ../include/RTI/LogicalTimeInterval.h
   ../include/RTI/NullFederateAmbassador.h
   ../include/RTI/RangeBounds.h
   ../include/RTI/RTI1516.h
   ../include/RTI/RTIambassador.h
   ../include/RTI/RTIambassadorFactory.h
   ../include/RTI/SpecificConfig.h
   ../include/RTI/Typedefs.h
   ../include/RTI/VariableLengthData.h
)

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


ADD_LIBRARY(RTI1516 ${RTI1516_LIB_SRCS} ${RTI1516_LIB_INCLUDE})
# Incorrect line
#TARGET_LINK_LIBRARIES(RTI1516 CERTI)
# Correct line
TARGET_LINK_LIBRARIES(RTI1516 CERTI FedTime1516)

MESSAGE(STATUS "libRTI variant: HLA 1516")
SET_TARGET_PROPERTIES(RTI1516 PROPERTIES OUTPUT_NAME "RTI1516")
SET_TARGET_PROPERTIES(RTI1516 PROPERTIES COMPILE_FLAGS "-DHLA13NG_LIBRTI -DRTI_DISABLE_WARNINGS -DBUILDING_RTI")
SET_TARGET_PROPERTIES(RTI1516 PROPERTIES VERSION 1.0.0 SOVERSION 1)
IF (MINGW)
    SET_TARGET_PROPERTIES(RTI1516 PROPERTIES LINK_FLAGS "-Wl,--output-def,${LIBRARY_OUTPUT_PATH}/libRTI1516.def")
    INSTALL(FILES ${LIBRARY_OUTPUT_PATH}/libRTI1516.def
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

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/libHLA)
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

SET(TestFedTime_SRCS
  TestFedTime.cc
)
ADD_EXECUTABLE(TestFedTime ${TestFedTime_SRCS})
TARGET_LINK_LIBRARIES(TestFedTime FedTime RTI)
ADD_TEST(TestLibFedTime TestFedTime)

##########################################################
SET(FEDTIME1516_SRCS
    ../include/RTI/certiLogicalTime.h
    ../include/RTI/certiLogicalTimeInterval.h
    ../include/RTI/certiLogicalTimeFactory.h
    RTI1516fedTime.h
    RTI1516fedTime.cpp

)
IF (BUILD_WITH_EXTRA_LOGICAL_TIME)
   LIST(APPEND FEDTIME1516_SRC
        LogicalTimeDouble.h
        LogicalTimeDouble.cpp
        )
ENDIF(BUILD_WITH_EXTRA_LOGICAL_TIME)

SOURCE_GROUP("Source Files\\FedTime" FILES ${FEDTIME1516_SRCS})

ADD_LIBRARY(FedTime1516
    ${FEDTIME1516_SRCS}
)

# Correct line
TARGET_LINK_LIBRARIES(FedTime1516)
# Incorrect line
#TARGET_LINK_LIBRARIES(FedTime1516 RTI1516)

SET_TARGET_PROPERTIES(FedTime1516 PROPERTIES PROJECT_LABEL LibFedTime1516)
SET_TARGET_PROPERTIES(FedTime1516 PROPERTIES COMPILE_FLAGS "-DBUILDING_FEDTIME -DRTI_DISABLE_WARNINGS")

IF (MINGW)
    SET_TARGET_PROPERTIES(FedTime1516 PROPERTIES LINK_FLAGS "-Wl,--output-def,${LIBRARY_OUTPUT_PATH}/libFedTime1516.def")
    INSTALL(FILES ${LIBRARY_OUTPUT_PATH}/libFedTime1516.def
            DESTINATION lib)
ENDIF (MINGW)

SET_TARGET_PROPERTIES(FedTime1516 PROPERTIES VERSION 1.0.0 SOVERSION 1)

SET(TestFedTime1516_SRCS
  TestFedTime.cc
)

#ADD_EXECUTABLE(TestFedTime1516 ${TestFedTime1516_SRCS})

# TODO: Should link with RTI1516 lib, instead of base RTI lib
#TARGET_LINK_LIBRARIES(TestFedTime1516 FedTime1516 RTI)

#ADD_TEST(TestLibFedTime1516 TestFedTime1516)


# Install rules for both libRTI-NG and libFedTime
INSTALL(TARGETS RTI FedTime RTI1516 FedTime1516
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)

INSTALL(FILES RTI1516fedTime.h DESTINATION include/RTI)    
