## $Id: Makefile.am,v 3.29 2007/07/25 14:35:07 erk Exp $ 

MAINTAINERCLEANFILES = Makefile.in

CLEANFILES = syntax.cc syntax.h tokens.cc

INCLUDES = -I$(top_srcdir)/include @XML_CFLAGS@ @HAVE_XML@ @WITH_CYGWIN@

lib_LTLIBRARIES = libCERTI.la

noinst_HEADERS = AuditFile.hh AuditLine.hh BaseRegion.hh		\
BasicMessage.hh Dimension.hh Exception.hh Extent.hh	\
FederateLevelList.hh fed.hh FedRegion.hh GAV.hh GSSAPIHandler.hh	\
Handled.hh HandleManager.hh helper.hh InteractionBroadcastList.hh	\
Interaction.hh InteractionSet.hh LBTS.hh MessageBody.hh Message.hh	\
Named.hh NetworkMessage.hh ObjectAttribute.hh ObjectClassAttribute.hh	\
ObjectClassBroadcastList.hh ObjectClass.hh ObjectClassSet.hh Object.hh	\
ObjectSet.hh Parameter.hh RootObject.hh RoutingSpace.hh RTIRegion.hh	\
SecureTCPSocket.hh SecurityLevel.hh SecurityServer.hh Socket.hh		\
SocketMC.hh SocketServer.hh SocketTCP.hh SocketUDP.hh SocketUN.hh	\
XmlParser.hh Subscribable.hh DebugOStream.hh PrettyDebug.hh Socket.hh	\
SocketUN.hh

libCERTI_la_SOURCES = AuditFile.cc AuditLine.cc BaseRegion.cc		\
BasicMessage.cc Dimension.cc Exception.cc Extent.cc	\
fed.cc FederateLevelList.cc FedRegion.cc FedTime.cc GAV_aux.cc GAV.cc	\
GSSAPIHandler.cc InteractionBroadcastList.cc Interaction.cc		\
InteractionSet.cc LBTS.cc MessageBody.cc Message.cc Message_RW.cc	\
Named.cc NetworkMessage.cc NetworkMessage_RW.cc ObjectAttribute.cc	\
Object.cc ObjectClassAttribute.cc ObjectClassBroadcastList.cc		\
ObjectClass.cc ObjectClassSet.cc ObjectSet.cc Parameter.cc		\
RootObject.cc RoutingSpace.cc RTIRegion.cc SecureTCPSocket.cc		\
SecurityServer.cc SocketMC.cc SocketServer.cc SocketTCP.cc		\
SocketUDP.cc SocketUN.cc syntax.yy tokens.ll XmlParser.cc		\
Subscribable.cc PrettyDebug.cc PrettyDebEx.cc \
Clock.cc PosixClock.cc TSCClock.cc

libCERTI_la_LDFLAGS = -version-info 1:0:0

EXTRA_DIST = syntax.h CMakeLists.txt
