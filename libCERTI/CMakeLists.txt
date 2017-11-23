# LibCERTI now depends on libHLA
include_directories(${CERTI_SOURCE_DIR})

##################################################################
# Lib CERTI
# The CERTI library is the internal CERTI library it should
# not depends on _ANY_ RTI libs, neither libRTI nor libFedTime.
# because it will end-up in an unwanted circular reference.
##################################################################
set(CERTI_FEDERATION_SRCS
    fed.cc fed.hh
    FederateLevelList.cc FederateLevelList.hh
    FedRegion.cc FedRegion.hh
)

set(CERTI_DECLARATION_SRCS
    InteractionBroadcastList.cc InteractionBroadcastList.hh
    Interaction.cc Interaction.hh
    InteractionSet.cc InteractionSet.hh
)

set(CERTI_OBJECT_SRCS
    ObjectAttribute.cc ObjectAttribute.hh
    Object.cc Object.hh
    ObjectClassAttribute.cc ObjectClassAttribute.hh
    ObjectClassBroadcastList.cc ObjectClassBroadcastList.hh
    ObjectClass.cc ObjectClass.hh
    ObjectClassSet.cc ObjectClassSet.hh
    ObjectSet.cc ObjectSet.hh
    Named.cc Named.hh
    NameReservation.hh
    Parameter.cc Parameter.hh
    RootObject.cc RootObject.hh
    Subscribable.cc Subscribable.hh
)

set(CERTI_OWNERSHIP_SRCS
    GAV.cc GAV.hh
)

set(CERTI_DDM_SRCS
    RTIRegion.cc RTIRegion.hh
    BaseRegion.cc BaseRegion.hh
    Dimension.cc Dimension.hh
    Extent.cc Extent.hh
    RoutingSpace.cc RoutingSpace.hh
)

set(CERTI_TIME_SRCS
    FedTime.cc FedTimeD.hh
    LBTS.cc LBTS.hh
)

set(CERTI_SUPPORT_SRCS
    AuditFile.cc AuditFile.hh
    AuditLine.cc AuditLine.hh
    BasicMessage.cc BasicMessage.hh
    M_Classes.cc M_Classes.hh # These files are generated
    Message.cc Message_RW.cc Message.hh 
    NetworkMessage.cc NetworkMessage_RW.cc NetworkMessage.hh
    NM_Classes.hh NM_Classes.cc # These files are generated
    Exception.cc Exception.hh
    XmlParser.cc XmlParser.hh
    XmlParser2000.cc XmlParser2000.hh
    XmlParser2010.cc XmlParser2010.hh
    PrettyDebug.cc PrettyDebug.hh
    TreeNamedAndHandledSet.hh
    StrongType.hh
    Handle.hh
    MessageEvent.hh
)

set(CERTI_SOCKET_SRCS
    SocketServer.cc SocketServer.hh
    SocketTCP.cc SocketTCP.hh
    SecureTCPSocket.cc SecureTCPSocket.hh
    SecurityServer.cc SecurityServer.hh
    SocketHTTPProxy.cc SocketHTTPProxy.hh
    GSSAPIHandler.cc GSSAPIHandler.hh
    Socket.hh
)

add_definitions(-DDEBUG)
set(CERTI_SOCKET_SHM_SRC
    SocketSHM.cc SocketSHM.hh
    RingBuffer.cc RingBuffer.hh
    )
if (WIN32)    
    list(APPEND CERTI_SOCKET_SHM_SRC
        SocketSHMWin32.cc SocketSHMWin32.hh
        )
else(WIN32)    
    add_definitions(-DDEBUG)
    list(APPEND CERTI_SOCKET_SHM_SRC
        SocketSHMPosix.cc SocketSHMPosix.hh
        SocketSHMSysV.cc SocketSHMSysV.hh
        )
endif(WIN32)
list(APPEND CERTI_SOCKET_SRCS ${CERTI_SOCKET_SHM_SRC})

set(CERTI_SOCKET_SRCS ${CERTI_SOCKET_SRCS} SocketUDP.cc SocketMC.cc SocketUN.cc SocketUDP.hh SocketMC.hh SocketUN.hh)
if (WIN32)
    set(CERTI_SOCKET_SRCS ${CERTI_SOCKET_SRCS} socketpair_win32.c)
endif (WIN32)

source_group("Source Files\\Socket" FILES ${CERTI_SOCKET_SRCS})
source_group("Source Files\\Federation" FILES ${CERTI_FEDERATION_SRCS})
source_group("Source Files\\Declaration" FILES ${CERTI_DECLARATION_SRCS})
source_group("Source Files\\Object" FILES ${CERTI_OBJECT_SRCS})
source_group("Source Files\\OwnerShip" FILES ${CERTI_OWNERSHIP_SRCS})
source_group("Source Files\\DDM" FILES ${CERTI_DDM_SRCS})
source_group("Source Files\\Time" FILES ${CERTI_TIME_SRCS})
source_group("Source Files\\Support" FILES ${CERTI_SUPPORT_SRCS})

set(CERTI_GENERATED_SRCS "")

set(YACC_SRC               ${CMAKE_CURRENT_SOURCE_DIR}/syntax.yy)
set(YACC_OUT_PREFIX        ${CMAKE_CURRENT_BINARY_DIR}/y.tab)
set(YACC_WANTED_OUT_PREFIX ${CMAKE_CURRENT_BINARY_DIR}/syntax)

set(LEX_SRC               ${CMAKE_CURRENT_SOURCE_DIR}/tokens.ll)
set(LEX_OUT_PREFIX        ${CMAKE_CURRENT_BINARY_DIR}/tokens_yy)
set(LEX_WANTED_OUT_PREFIX ${CMAKE_CURRENT_BINARY_DIR}/tokens)

#Exec Lex
add_custom_command(
   OUTPUT  ${LEX_WANTED_OUT_PREFIX}.cc
   COMMAND ${LEX_PROGRAM} ARGS -o${LEX_WANTED_OUT_PREFIX}.cc ${LEX_SRC}
   DEPENDS ${LEX_SRC}
   )
set(CERTI_GENERATED_SRCS ${CERTI_GENERATED_SRCS} ${LEX_WANTED_OUT_PREFIX}.cc)

#Exec Yacc
add_custom_command(
   OUTPUT  ${YACC_WANTED_OUT_PREFIX}.cc ${YACC_WANTED_OUT_PREFIX}.h
   COMMAND ${YACC_PROGRAM} ARGS ${YACC_COMPAT_ARG} -d ${YACC_SRC}
   COMMAND ${CMAKE_COMMAND} -E copy ${YACC_OUT_PREFIX}.h  ${YACC_WANTED_OUT_PREFIX}.h
   COMMAND ${CMAKE_COMMAND} -E copy ${YACC_OUT_PREFIX}.c  ${YACC_WANTED_OUT_PREFIX}.cc
   DEPENDS ${YACC_SRC}
   )
   
# Inhibit compiler warning for LEX/YACC generated files
# Note that the inhibition is COMPILER dependent ...
# GNU CC specific warning stop
if (CMAKE_COMPILER_IS_GNUCC) 
   message(STATUS "INHIBIT Compiler warning for LEX/YACC generated files")
   set_source_files_properties(${YACC_WANTED_OUT_PREFIX}.cc ${YACC_WANTED_OUT_PREFIX}.h
                               PROPERTIES COMPILE_FLAGS "-w")
   set_source_files_properties(${LEX_WANTED_OUT_PREFIX}.cc
                               PROPERTIES COMPILE_FLAGS "-w") 
endif(CMAKE_COMPILER_IS_GNUCC)
set(CERTI_GENERATED_SRCS ${CERTI_GENERATED_SRCS} ${YACC_WANTED_OUT_PREFIX}.cc)
set(CERTI_GENERATED_SRCS ${CERTI_GENERATED_SRCS} ${PROJECT_BINARY_DIR}/config.h)
source_group("Source Files\\Generated" FILES ${CERTI_GENERATED_SRCS})

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# FIXME we should FindGnuWin32 in the main CMakeList.txt
if (WIN32 AND NOT CMAKE_CROSSCOMPILING)
    include_directories("C:/Program Files/GnuWin32/include")
endif ()

add_library(CERTI
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

set_target_properties(CERTI PROPERTIES PROJECT_LABEL LibCERTI)


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
        set_property(TARGET CERTI PROPERTY CXX_STANDARD 14)
    endif()
endif()


target_link_libraries(CERTI
    ${LIBXML2_LIBRARIES}
    ${GEN_LIBRARY}
    ${SOCKET_LIBRARY} HLA)
if (MINGW)
    set_target_properties(CERTI PROPERTIES LINK_FLAGS "-Wl,--output-def,${LIBRARY_OUTPUT_PATH}/libCERTI.def")
    install(FILES ${LIBRARY_OUTPUT_PATH}/libCERTI.def
            DESTINATION lib)
endif (MINGW)

set_target_properties(CERTI PROPERTIES VERSION ${PACKAGE_VERSION} SOVERSION ${PACKAGE_VERSION_MAJOR})

install(TARGETS CERTI
        EXPORT CERTIDepends
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)
