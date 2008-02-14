
SET(CERTI_FEDERATION_SRCS
    fed.cc fed.hh
    FederateLevelList.cc FederateLevelList.hh
    FedRegion.cc FedRegion.hh
)
 
SET(CERTI_DECLARATION_SRCS
    InteractionBroadcastList.cc
    Interaction.cc
    InteractionSet.cc
    InteractionBroadcastList.hh
    Interaction.hh
    InteractionSet.hh
)
 
SET(CERTI_OBJECT_SRCS
    ObjectAttribute.cc
    Object.cc
    ObjectClassAttribute.cc
    ObjectClassBroadcastList.cc
    ObjectClass.cc
    ObjectClassSet.cc
    ObjectSet.cc
    Named.cc
    Parameter.cc
    RootObject.cc
    Subscribable.cc
    ObjectAttribute.hh
    Object.hh
    ObjectClassAttribute.hh
    ObjectClassBroadcastList.hh
    ObjectClass.hh
    ObjectClassSet.hh
    ObjectSet.hh
    Named.hh
    Parameter.hh
    RootObject.hh
    Subscribable.hh
)

SET(CERTI_OWNERSHIP_SRCS
GAV_aux.cc
GAV.cc
GAV.hh
)

SET(CERTI_DDM_SRCS
RTIRegion.cc
BaseRegion.cc
Dimension.cc
Extent.cc
RoutingSpace.cc
RTIRegion.hh
BaseRegion.hh
Dimension.hh
Extent.hh
RoutingSpace.hh
)

SET(CERTI_TIME_SRCS
FedTime.cc
LBTS.cc
LBTS.hh
)

SET(CERTI_SUPPORT_SRCS
AuditFile.cc AuditFile.hh
AuditLine.cc AuditLine.hh
BasicMessage.cc BasicMessage.hh
Clock.cc Clock.hh
MessageBody.cc MessageBody.hh
MessageBuffer.cc MessageBuffer.hh
Message.cc Message_R.cc Message_W.cc Message.hh
NetworkMessage.cc NetworkMessage_RW.cc NetworkMessage.hh
Exception.cc Exception.hh
XmlParser.cc XmlParser.hh
PrettyDebug.cc PrettyDebEx.cc PrettyDebug.hh
)

IF (HAVE_POSIX_CLOCK)
    SET(CERTI_SUPPORT_SRCS 
           ${CERTI_SUPPORT_SRCS}
           PosixClock.cc PosixClock.hh)              
ENDIF(HAVE_POSIX_CLOCK)

IF (HAVE_TSC_CLOCK)
    SET(CERTI_SUPPORT_SRCS 
           ${CERTI_SUPPORT_SRCS}
           TSCClock.cc TSCClock.hh)              
ENDIF(HAVE_TSC_CLOCK)

SET(CERTI_SOCKET_SRCS
SocketServer.cc
SocketTCP.cc
SecureTCPSocket.cc
SecurityServer.cc
GSSAPIHandler.cc
SocketServer.hh
SocketTCP.hh
SecureTCPSocket.hh
SecurityServer.hh
GSSAPIHandler.hh
Socket.hh
)


SET(CERTI_SOCKET_SRCS ${CERTI_SOCKET_SRCS} SocketUDP.cc SocketMC.cc SocketUN.cc SocketUDP.hh SocketMC.hh SocketUN.hh)

SOURCE_GROUP("Source Files\\Socket" FILES ${CERTI_SOCKET_SRCS})
SOURCE_GROUP("Source Files\\Federation" FILES ${CERTI_FEDERATION_SRCS})
SOURCE_GROUP("Source Files\\Declaration" FILES ${CERTI_DECLARATION_SRCS})
SOURCE_GROUP("Source Files\\Object" FILES ${CERTI_OBJECT_SRCS})
SOURCE_GROUP("Source Files\\OwnerShip" FILES ${CERTI_OWNERSHIP_SRCS})
SOURCE_GROUP("Source Files\\DDM" FILES ${CERTI_DDM_SRCS})
SOURCE_GROUP("Source Files\\Time" FILES ${CERTI_TIME_SRCS})
SOURCE_GROUP("Source Files\\Support" FILES ${CERTI_SUPPORT_SRCS})

SET(CERTI_GENERATED_SRCS "")

SET(YACC_SRC               ${CMAKE_CURRENT_SOURCE_DIR}/syntax.yy)
SET(YACC_OUT_PREFIX        ${CMAKE_CURRENT_BINARY_DIR}/y.tab)
SET(YACC_WANTED_OUT_PREFIX ${CMAKE_CURRENT_BINARY_DIR}/syntax)

SET(LEX_SRC               ${CMAKE_CURRENT_SOURCE_DIR}/tokens.ll)
SET(LEX_OUT_PREFIX        ${CMAKE_CURRENT_BINARY_DIR}/tokens_yy)
SET(LEX_WANTED_OUT_PREFIX ${CMAKE_CURRENT_BINARY_DIR}/tokens)


#Exec Lex
ADD_CUSTOM_COMMAND(
   OUTPUT  ${LEX_WANTED_OUT_PREFIX}.cc
   COMMAND ${LEX_PROGRAM} ARGS -o${LEX_WANTED_OUT_PREFIX}.cc ${LEX_SRC}
   DEPENDS ${LEX_SRC}
   )
SET(CERTI_GENERATED_SRCS ${CERTI_GENERATED_SRCS} ${LEX_WANTED_OUT_PREFIX}.cc)

#Exec Yacc
ADD_CUSTOM_COMMAND(
   OUTPUT  ${YACC_WANTED_OUT_PREFIX}.cc ${YACC_WANTED_OUT_PREFIX}.h
   COMMAND ${YACC_PROGRAM} ARGS ${YACC_COMPAT_ARG} -d ${YACC_SRC}
   COMMAND ${CMAKE_COMMAND} -E copy ${YACC_OUT_PREFIX}.h  ${YACC_WANTED_OUT_PREFIX}.h
   COMMAND ${CMAKE_COMMAND} -E copy ${YACC_OUT_PREFIX}.c  ${YACC_WANTED_OUT_PREFIX}.cc
   DEPENDS ${YACC_SRC}
   )
   
# Inhibit compiler warning for LEX/YACC generated files
# Note that the inhibition is COMPILER dependent ...
# GNU CC specific warning stop
IF (CMAKE_COMPILER_IS_GNUCC) 
   MESSAGE(STATUS "INHIBIT Compiler warning for LEX/YACC generated files")
   SET_SOURCE_FILES_PROPERTIES(${YACC_WANTED_OUT_PREFIX}.cc ${YACC_WANTED_OUT_PREFIX}.h
	                           PROPERTIES COMPILE_FLAGS "-w")
	                            
   SET_SOURCE_FILES_PROPERTIES(${LEX_WANTED_OUT_PREFIX}.cc
	                           PROPERTIES COMPILE_FLAGS "-w") 
ENDIF(CMAKE_COMPILER_IS_GNUCC) 	     
SET(CERTI_GENERATED_SRCS ${CERTI_GENERATED_SRCS} ${YACC_WANTED_OUT_PREFIX}.cc)
SET(CERTI_GENERATED_SRCS ${CERTI_GENERATED_SRCS} ${PROJECT_BINARY_DIR}/config.h)
SOURCE_GROUP("Source Files\\Generated" FILES ${CERTI_GENERATED_SRCS})

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

# FIXME we should FindGnuWin32 in the main CMakeList.txt
IF (WIN32)
    INCLUDE_DIRECTORIES("C:/Program Files/GnuWin32/include")
ENDIF (WIN32)

SET(CERTI_EXPORTED_INCLUDES
	../include/fedtime.hh)

ADD_LIBRARY(CERTI
    ${CERTI_FEDERATION_SRCS}
    ${CERTI_DECLARATION_SRCS}
    ${CERTI_OBJECT_SRCS}
    ${CERTI_OWNERSHIP_SRCS}
    ${CERTI_DDM_SRCS}
    ${CERTI_TIME_SRCS}
    ${CERTI_SUPPORT_SRCS}
    ${CERTI_SOCKET_SRCS}
    ${CERTI_GENERATED_SRCS}	
    ${CERTI_EXPORTED_INCLUDES}
)

SET_TARGET_PROPERTIES(CERTI PROPERTIES PROJECT_LABEL LibCERTI)
                        
TARGET_LINK_LIBRARIES(CERTI ${LIBXML2_LIBRARIES} ${SOCKET_LIBRARY})
IF (MINGW)
    SET_TARGET_PROPERTIES(CERTI PROPERTIES LINK_FLAGS "-Wl,--output-def,${LIBRARY_OUTPUT_PATH}/libCERTI.def")
    INSTALL(FILES ${LIBRARY_OUTPUT_PATH}/libCERTI.def
            DESTINATION lib)
ENDIF (MINGW)

IF (HAVE_POSIX_CLOCK)
    TARGET_LINK_LIBRARIES(CERTI rt)
ENDIF (HAVE_POSIX_CLOCK)

SET_TARGET_PROPERTIES(CERTI PROPERTIES VERSION 1.0.0 SOVERSION 1)

INSTALL(TARGETS CERTI 
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)
