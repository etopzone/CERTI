## CERTI libCERTI Makefile.am
## $Id: Makefile.am,v 3.16 2005/03/25 17:08:03 breholee Exp $ 
## ---------------------------------------------------------------------------

MAINTAINERCLEANFILES = Makefile.in

INCLUDES = -I$(top_srcdir)/include -I$(top_srcdir)/libpdebug	\
@XML_CFLAGS@ @HAVE_XML@

lib_LTLIBRARIES = libCERTI.la

include_HEADERS = Socket.hh SocketUN.hh

noinst_HEADERS = AuditFile.hh AuditLine.hh BaseRegion.hh		\
BasicMessage.hh converter.hh Dimension.hh Exception.hh Extent.hh	\
FederateLevelList.hh fed.hh FedRegion.hh GAV.hh GSSAPIHandler.hh	\
Handled.hh HandleManager.hh helper.hh InteractionBroadcastLine.hh	\
InteractionBroadcastList.hh Interaction.hh InteractionSet.hh LBTS.hh	\
MessageBody.hh Message.hh Named.hh NetworkMessage.hh			\
ObjectAttribute.hh ObjectBroadcastLine.hh ObjectClassAttribute.hh	\
ObjectClassBroadcastList.hh ObjectClass.hh ObjectClassSet.hh Object.hh	\
ObjectSet.hh Parameter.hh Publisher.hh RootObject.hh RoutingSpace.hh	\
RTIRegion.hh SecureTCPSocket.hh SecurityLevel.hh SecurityServer.hh	\
Socket.hh SocketMC.hh SocketServer.hh SocketTCP.hh SocketUDP.hh		\
SocketUN.hh Subscriber.hh XmlParser.hh

libCERTI_la_SOURCES = AuditFile.cc AuditLine.cc BaseRegion.cc		\
BasicMessage.cc converter.cc Dimension.cc Exception.cc Extent.cc	\
fed.cc FederateLevelList.cc FedRegion.cc FedTime.cc GAV_aux.cc GAV.cc	\
GSSAPIHandler.cc InteractionBroadcastList.cc Interaction.cc		\
InteractionSet.cc LBTS.cc MessageBody.cc Message.cc Message_RW.cc	\
Named.cc NetworkMessage.cc NetworkMessage_RW.cc ObjectAttribute.cc	\
Object.cc ObjectClassAttribute.cc ObjectClassBroadcastList.cc		\
ObjectClass.cc ObjectClassSet.cc ObjectSet.cc Parameter.cc		\
Publisher.cc RootObject.cc RoutingSpace.cc RTIRegion.cc	\
SecureTCPSocket.cc SecurityServer.cc SocketMC.cc SocketServer.cc	\
SocketTCP.cc SocketUDP.cc SocketUN.cc Subscriber.cc syntax.cc		\
tokens.cc XmlParser.cc

# libCERTI_la_LDFLAGS = -version-info 0:0:0
# Too unstable to use -version-info, so :
libCERTI_la_LDFLAGS = -release 3.2

LDADD = @XML_LIBS@

EXTRA_DIST = syntax.h

## EOF $Id: Makefile.am,v 3.16 2005/03/25 17:08:03 breholee Exp $
