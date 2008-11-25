
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/libCERTI)

SET(RTI_LIB_SRCS
  RTIambassador.cc
  RTIambPrivateRefs.cc
  RTItypes.cc
  )
  
SET(RTI_LIB_INCLUDE
   ../include/baseTypes.hh
   ../include/certi.hh
   ../include/federateAmbServices.hh
   ../include/NullFederateAmbassador.hh
   ../include/RTI.hh
   ../include/RTIambServices.hh
   ../include/RTItypes.hh
)

#SOURCE_GROUP("RTI\\Header" FILES ${RTI_LIB_INCLUDE})

ADD_LIBRARY(RTI-NG ${RTI_LIB_SRCS} ${RTI_LIB_INCLUDE})
TARGET_LINK_LIBRARIES(RTI-NG CERTI)

SET_TARGET_PROPERTIES(RTI-NG PROPERTIES VERSION 1.0.0 SOVERSION 1)
IF (MINGW)
    SET_TARGET_PROPERTIES(RTI-NG PROPERTIES LINK_FLAGS "-Wl,--output-def,${LIBRARY_OUTPUT_PATH}/libRTI.def")
    INSTALL(FILES ${LIBRARY_OUTPUT_PATH}/libRTI.def
            DESTINATION lib)
ENDIF (MINGW)

INSTALL(TARGETS RTI-NG
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)
