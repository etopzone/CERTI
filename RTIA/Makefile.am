## CERTI RTIA Makefile.am
## Copyright (C) 2001, 2002, 2003 ONERA
## $Id: Makefile.am,v 3.5 2003/11/12 14:40:57 breholee Exp $
##
## Process this file with automake to produce Makefile.in

# @configure_input@

MAINTAINERCLEANFILES = Makefile.in

INCLUDES = -I$(top_srcdir)/include -I$(top_srcdir)/libCERTI	\
-I$(top_srcdir)/libpdebug @XML_CFLAGS@ @HAVE_XML@ @WITH_CYGWIN@

bin_PROGRAMS = rtia

noinst_HEADERS = Communications.hh DataDistribution.hh			\
DeclarationManagement.hh FederationManagement.hh Files.hh		\
ObjectManagement.hh OwnershipManagement.hh RTIA.hh Statistics.hh	\
TimeManagement.hh

rtia_SOURCES = Communications.cc DataDistribution.cc			\
DeclarationManagement.cc FederationManagement.cc Files.cc main.cc	\
ObjectManagement.cc OwnershipManagement.cc RTIA.cc RTIA_federate.cc	\
RTIA_network.cc Statistics.cc TimeManagement.cc

LDADD = -L$(top_builddir)/libCERTI -L$(top_builddir)/libpdebug -lCERTI	\
-lpdebug @ETR_SOCKET_LIBS@ @XML_LIBS@ @LEXLIB@

## $Id: Makefile.am,v 3.5 2003/11/12 14:40:57 breholee Exp $
