## CERTI libCERTI Makefile.am
## $Id: Makefile.am,v 3.12 2003/10/27 14:10:09 breholee Exp $ 
## ---------------------------------------------------------------------------

MAINTAINERCLEANFILES = Makefile.in

INCLUDES = -I$(top_srcdir)/include -I$(top_srcdir)/libpdebug	\
@XML_CFLAGS@ @HAVE_XML@

lib_LTLIBRARIES = libCERTI.la

include_HEADERS = Socket.hh SocketUN.hh

noinst_HEADERS = AuditFile.hh AuditLine.hh converter.hh Dimension.hh	\
Exception.hh Extent.hh FederateClock.hh FederateLevelList.hh GAV.hh	\
GSSAPIHandler.hh Interaction.hh InteractionBroadcastLine.hh		\
InteractionBroadcastList.hh InteractionSet.hh LBTS.hh Message.hh	\
MessageBody.hh NetworkMessage.hh Object.hh ObjectSet.hh			\
ObjectAttribute.hh ObjectBroadcastLine.hh ObjectClass.hh		\
ObjectClassAttribute.hh ObjectClassBroadcastList.hh ObjectClassSet.hh	\
Parameter.hh Publisher.hh Range.hh RegionImp.hh RootObject.hh		\
RoutingSpace.hh SecureTCPSocket.hh SecurityLevel.hh SecurityServer.hh	\
SocketMC.hh SocketServer.hh SocketTCP.hh SocketUDP.hh Subscriber.hh	\
XmlParser.hh fed.hh

libCERTI_la_SOURCES = AuditFile.cc AuditLine.cc Dimension.cc		\
Exception.cc Extent.cc FederateLevelList.cc FedTime.cc GAV.cc		\
GAV_aux.cc GSSAPIHandler.cc Interaction.cc InteractionBroadcastList.cc	\
InteractionSet.cc LBTS.cc Message.cc Message_RW.cc MessageBody.cc	\
NetworkMessage.cc NetworkMessage_RW.cc Object.cc ObjectSet.cc		\
ObjectAttribute.cc ObjectClass.cc ObjectClassAttribute.cc		\
ObjectClassBroadcastList.cc ObjectClassSet.cc Parameter.cc		\
Publisher.cc Range.cc RegionImp.cc RootObject.cc RoutingSpace.cc	\
SecureTCPSocket.cc SecurityServer.cc SocketMC.cc SocketServer.cc	\
SocketTCP.cc SocketUDP.cc SocketUN.cc Subscriber.cc XmlParser.cc	\
converter.cc fed.cc syntax.yy tokens.ll

# libCERTI_la_LDFLAGS = -version-info 0:0:0
# Too unstable to use -version-info, so :
libCERTI_la_LDFLAGS = -release 3.0.1

LDADD = @XML_LIBS@

EXTRA_DIST = syntax.h

## EOF $Id: Makefile.am,v 3.12 2003/10/27 14:10:09 breholee Exp $
