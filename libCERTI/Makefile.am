## $Id: Makefile.am,v 3.1 2002/11/26 15:48:01 breholee Exp $ 
## ---------------------------------------------------------------------------

MAINTAINERCLEANFILES = Makefile.in

INCLUDES = -I$(top_srcdir)/include -I$(top_srcdir)/libpdebug

lib_LTLIBRARIES = libCERTI.la

include_HEADERS = Socket.hh SocketUN.hh

noinst_HEADERS = audit.hh AuditFile.hh AuditLine.hh broadcast.hh	\
exception_type.hh fed_file.hh FederateClock.hh FederateLevelList.hh	\
FederateLevelTuple.hh FedParser.hh GSSAPIHandler.hh Interaction.hh	\
InteractionBroadcastLine.hh InteractionBroadcastList.hh			\
InteractionSet.hh LBTS.hh List.hh Message.hh message_types.hh		\
MessageBody.hh network.hh NetworkMessage.hh Object.hh			\
ObjectAttribute.hh ObjectBroadcastLine.hh ObjectClass.hh		\
ObjectClassAttribute.hh ObjectClassBroadcastList.hh ObjectClassSet.hh	\
Parameter.hh parser.hh Publisher.hh RootObject.hh SecureTCPSocket.hh	\
SecurityLevel.hh SecurityServer.hh service_type.hh		\
SocketMC.hh SocketServer.hh SocketTCP.hh SocketUDP.hh 	\
Subscriber.hh

libCERTI_la_SOURCES = AuditFile.cc AuditLine.cc baseTypes.cc		\
FederateLevelList.cc FedParser_Build.cc FedParser_Store.cc FedTime.cc	\
GAV.cc GAV_aux.cc GSSAPIHandler.cc Interaction.cc			\
InteractionBroadcastList.cc InteractionSet.cc LBTS.cc List.cc		\
Message.cc Message_RW.cc MessageBody.cc NetworkMessage.cc		\
NetworkMessage_RW.cc Object.cc ObjectAttribute.cc ObjectClass.cc	\
ObjectClassAttribute.cc ObjectClassBroadcastList.cc ObjectClassSet.cc	\
Parameter.cc RootObject.cc SecureTCPSocket.cc SecurityServer.cc		\
SocketMC.cc SocketServer.cc SocketTCP.cc SocketUDP.cc SocketUN.cc	\
Subscriber.cc

# libCERTI_la_LDFLAGS = -version-info 0:0:0
# Too unstable to use -version-info, so :
libCERTI_la_LDFLAGS = -release 3.0.1

## EOF $Id: Makefile.am,v 3.1 2002/11/26 15:48:01 breholee Exp $
