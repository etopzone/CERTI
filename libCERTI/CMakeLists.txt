
SET(CERTI_LIB_SRCS
  AuditFile.cc
  AuditLine.cc
  BaseRegion.cc
  BasicMessage.cc
  Dimension.cc
  Exception.cc
  Extent.cc
  fed.cc
  FederateLevelList.cc
  FedRegion.cc
  FedTime.cc
  GAV_aux.cc
  GAV.cc
  GSSAPIHandler.cc
  InteractionBroadcastList.cc
  Interaction.cc
  InteractionSet.cc
  LBTS.cc
  MessageBody.cc
  Message.cc
  Message_RW.cc
  Named.cc
  NetworkMessage.cc
  NetworkMessage_RW.cc
  ObjectAttribute.cc
  Object.cc
  ObjectClassAttribute.cc
  ObjectClassBroadcastList.cc
  ObjectClass.cc
  ObjectClassSet.cc
  ObjectSet.cc
  Parameter.cc
  RootObject.cc
  RoutingSpace.cc
  RTIRegion.cc
  SecureTCPSocket.cc
  SecurityServer.cc
  SocketMC.cc
  SocketServer.cc
  SocketTCP.cc
  SocketUDP.cc
  SocketUN.cc
  XmlParser.cc
  Subscribable.cc
  PrettyDebug.cc
  )

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

SET(CERTI_LIB_SRCS ${CERTI_LIB_SRCS} ${LEX_WANTED_OUT_PREFIX}.cc)

#Exec Yacc
ADD_CUSTOM_COMMAND(
   OUTPUT  ${YACC_WANTED_OUT_PREFIX}.cc ${YACC_WANTED_OUT_PREFIX}.h
   COMMAND ${YACC_PROGRAM} ARGS ${YACC_COMPAT_ARG} -d ${YACC_SRC}
   COMMAND ${CMAKE_COMMAND} -E copy ${YACC_OUT_PREFIX}.h  ${YACC_WANTED_OUT_PREFIX}.h
   COMMAND ${CMAKE_COMMAND} -E copy ${YACC_OUT_PREFIX}.c  ${YACC_WANTED_OUT_PREFIX}.cc
   DEPENDS ${YACC_SRC}
   )

SET(CERTI_LIB_SRCS ${CERTI_LIB_SRCS} ${YACC_WANTED_OUT_PREFIX}.cc)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

ADD_LIBRARY(CERTI SHARED ${CERTI_LIB_SRCS})
TARGET_LINK_LIBRARIES(CERTI ${LIBXML2_LIBRARIES})

#SET_TARGET_PROPERTIES(CERTI PROPERTIES VERSION 1.0.0 SOVERSION 0 )

INSTALL(TARGETS CERTI 
  DESTINATION lib )


########### install files ###############

#original Makefile.am contents follow:

### $Id: CMakeLists.txt,v 1.2 2007/05/03 08:24:00 erk Exp $ 
#
#MAINTAINERCLEANFILES = Makefile.in
#
#CLEANFILES = syntax.cc syntax.h tokens.cc
#
#INCLUDES = -I$(top_srcdir)/include @XML_CFLAGS@ @HAVE_XML@ @WITH_CYGWIN@
#
#lib_LTLIBRARIES = libCERTI.la
#
#noinst_HEADERS = AuditFile.hh AuditLine.hh BaseRegion.hh		\
#BasicMessage.hh Dimension.hh Exception.hh Extent.hh	\
#FederateLevelList.hh fed.hh FedRegion.hh GAV.hh GSSAPIHandler.hh	\
#Handled.hh HandleManager.hh helper.hh InteractionBroadcastList.hh	\
#Interaction.hh InteractionSet.hh LBTS.hh MessageBody.hh Message.hh	\
#Named.hh NetworkMessage.hh ObjectAttribute.hh ObjectClassAttribute.hh	\
#ObjectClassBroadcastList.hh ObjectClass.hh ObjectClassSet.hh Object.hh	\
#ObjectSet.hh Parameter.hh RootObject.hh RoutingSpace.hh RTIRegion.hh	\
#SecureTCPSocket.hh SecurityLevel.hh SecurityServer.hh Socket.hh		\
#SocketMC.hh SocketServer.hh SocketTCP.hh SocketUDP.hh SocketUN.hh	\
#XmlParser.hh Subscribable.hh DebugOStream.hh PrettyDebug.hh Socket.hh	\
#SocketUN.hh
#
#libCERTI_la_SOURCES = AuditFile.cc AuditLine.cc BaseRegion.cc		\
#BasicMessage.cc Dimension.cc Exception.cc Extent.cc	\
#fed.cc FederateLevelList.cc FedRegion.cc FedTime.cc GAV_aux.cc GAV.cc	\
#GSSAPIHandler.cc InteractionBroadcastList.cc Interaction.cc		\
#InteractionSet.cc LBTS.cc MessageBody.cc Message.cc Message_RW.cc	\
#Named.cc NetworkMessage.cc NetworkMessage_RW.cc ObjectAttribute.cc	\
#Object.cc ObjectClassAttribute.cc ObjectClassBroadcastList.cc		\
#ObjectClass.cc ObjectClassSet.cc ObjectSet.cc Parameter.cc		\
#RootObject.cc RoutingSpace.cc RTIRegion.cc SecureTCPSocket.cc		\
#SecurityServer.cc SocketMC.cc SocketServer.cc SocketTCP.cc		\
#SocketUDP.cc SocketUN.cc syntax.yy tokens.ll XmlParser.cc		\
#Subscribable.cc PrettyDebug.cc
#
#libCERTI_la_LDFLAGS = -version-info 1:0:0
#
#EXTRA_DIST = syntax.h
