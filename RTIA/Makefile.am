## CERTI RTIA Makefile.am
## Copyright (C) 2001, 2002, 2003 ONERA
## $Id: Makefile.am,v 3.1 2003/02/17 09:17:03 breholee Exp $
##
## Process this file with automake to produce Makefile.in

# @configure_input@

MAINTAINERCLEANFILES = Makefile.in

INCLUDES = -I$(top_srcdir)/include -I$(top_srcdir)/libCERTI	\
-I$(top_srcdir)/libpdebug @XML_CFLAGS@ @HAVE_XML@

bin_PROGRAMS = rtia

noinst_HEADERS = Communications.hh DeclarationManagement.hh		\
FederationManagement.hh Files.hh ObjectManagement.hh			\
OwnershipManagement.hh RTIA.hh TimeManagement.hh DataDistribution.hh

rtia_SOURCES = Communications.cc DeclarationManagement.cc	\
FederationManagement.cc Files.cc main.cc ObjectManagement.cc	\
OwnershipManagement.cc RTIA.cc RTIA_federate.cc RTIA_network.cc	\
TimeManagement.cc DataDistribution.cc

LDADD = -L$(top_builddir)/libCERTI -L$(top_builddir)/libpdebug -lCERTI	\
-lpdebug @ETR_SOCKET_LIBS@ @XML_LIBS@

## $Id: Makefile.am,v 3.1 2003/02/17 09:17:03 breholee Exp $
