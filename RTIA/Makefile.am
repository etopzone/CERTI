## CERTI rtia/Makefile.am 
## Copyright (C) 2001, 2002 Onera
## $Id: Makefile.am,v 3.0 2002/11/21 01:27:51 breholee Exp $
##
## Process this file with automake to produce Makefile.in

# @configure_input@

MAINTAINERCLEANFILES = Makefile.in

INCLUDES = -I$(top_srcdir)/include -I$(top_srcdir)/libCERTI	\
-I$(top_srcdir)/libpdebug

bin_PROGRAMS = rtia

noinst_HEADERS = Communications.hh DeclarationManagement.hh	\
FederationManagement.hh Files.hh ObjectManagement.hh		\
OwnershipManagement.hh RTIA.hh TimeManagement.hh

rtia_SOURCES = Communications.cc DeclarationManagement.cc	\
FederationManagement.cc Files.cc main.cc ObjectManagement.cc	\
OwnershipManagement.cc RTIA.cc RTIA_federate.cc RTIA_network.cc	\
TimeManagement.cc

LDADD = -L$(top_builddir)/libCERTI -L$(top_builddir)/libpdebug -lCERTI	\
-lpdebug @ETR_SOCKET_LIBS@

## EOF $Id: Makefile.am,v 3.0 2002/11/21 01:27:51 breholee Exp $


