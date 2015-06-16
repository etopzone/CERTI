# LibCERTI now depends on libHLA
include_directories(${CERTI_SOURCE_DIR}/libHLA)

##################################################################
# Lib CERTI
# The CERTI library is the internal CERTI library it should
# not depends on _ANY_ RTI libs, neither libRTI nor libFedTime.
# because it will end-up in an unwanted circular reference.
##################################################################
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
    NameReservation.cc
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
    NameReservation.hh
    Parameter.hh
    RootObject.hh
    Subscribable.hh
)

SET(CERTI_OWNERSHIP_SRCS
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
FedTime.cc FedTimeD.hh
LBTS.cc
LBTS.hh
)

SET(CERTI_SUPPORT_SRCS
AuditFile.cc AuditFile.hh
AuditLine.cc AuditLine.hh
BasicMessage.cc BasicMessage.hh
M_Classes.cc M_Classes.hh
Message.cc Message_RW.cc Message.hh 
NetworkMessage.cc NetworkMessage_RW.cc NetworkMessage.hh
NM_Classes.hh NM_Classes.cc
Exception.cc Exception.hh
XmlParser.cc XmlParser.hh
XmlParser2000.cc XmlParser2000.hh
XmlParser2010.cc XmlParser2010.hh
PrettyDebug.cc PrettyDebug.hh
TreeNamedAndHandledSet.hh
)

SET(CERTI_SOCKET_SRCS
SocketServer.cc
SocketTCP.cc
SecureTCPSocket.cc
SecurityServer.cc
SocketHTTPProxy.cc SocketHTTPProxy.hh
GSSAPIHandler.cc
SocketServer.hh
SocketTCP.hh
SecureTCPSocket.hh
SecurityServer.hh
GSSAPIHandler.hh
Socket.hh
)

if (WIN32)    
    ADD_DEFINITIONS(-DDEBUG)

    SET(CERTI_SOCKET_SHM_SRC
        SocketSHM.cc SocketSHM.hh
        SocketSHMWin32.cc SocketSHMWin32.hh
	    RingBuffer.cc RingBuffer.hh)

    LIST(APPEND CERTI_SOCKET_SRCS ${CERTI_SOCKET_SHM_SRC})
else(WIN32)    
    ADD_DEFINITIONS(-DDEBUG)

    SET(CERTI_SOCKET_SHM_SRC
        SocketSHM.cc SocketSHM.hh
        SocketSHMPosix.cc SocketSHMPosix.hh
        SocketSHMSysV.cc SocketSHMSysV.hh
	    RingBuffer.cc RingBuffer.hh
        )

    LIST(APPEND CERTI_SOCKET_SRCS ${CERTI_SOCKET_SHM_SRC})
endif(WIN32)

SET(CERTI_SOCKET_SRCS ${CERTI_SOCKET_SRCS} SocketUDP.cc SocketMC.cc SocketUN.cc SocketUDP.hh SocketMC.hh SocketUN.hh)
IF (WIN32)
    SET(CERTI_SOCKET_SRCS ${CERTI_SOCKET_SRCS} socketpair_win32.c)
ENDIF (WIN32)

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
if (WIN32 AND NOT CMAKE_CROSSCOMPILING)
    INCLUDE_DIRECTORIES("C:/Program Files/GnuWin32/include")
endif ()

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
)

SET_TARGET_PROPERTIES(CERTI PROPERTIES PROJECT_LABEL LibCERTI)

TARGET_LINK_LIBRARIES(CERTI
    ${LIBXML2_LIBRARIES}
    ${GEN_LIBRARY}
    ${SOCKET_LIBRARY} HLA)
IF (MINGW)
    SET_TARGET_PROPERTIES(CERTI PROPERTIES LINK_FLAGS "-Wl,--output-def,${LIBRARY_OUTPUT_PATH}/libCERTI.def")
    INSTALL(FILES ${LIBRARY_OUTPUT_PATH}/libCERTI.def
            DESTINATION lib)
ENDIF (MINGW)

SET_TARGET_PROPERTIES(CERTI PROPERTIES VERSION ${PACKAGE_VERSION} SOVERSION ${PACKAGE_VERSION_MAJOR})

INSTALL(TARGETS CERTI
    EXPORT CERTIDepends
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)
