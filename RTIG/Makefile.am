## CERTI rtig/Makefile.am 
## Copyright (C) 2001, 2002 ONERA
## $Id: Makefile.am,v 3.0 2002/11/21 01:27:51 breholee Exp $
##
## Process this file with automake to produce Makefile.in

# @configure_input@

MAINTAINERCLEANFILES = Makefile.in

INCLUDES = -I$(top_srcdir)/include -I$(top_srcdir)/libCERTI	\
-I$(top_srcdir)/libpdebug

bin_PROGRAMS = rtig

noinst_HEADERS = Federate.hh Federation.hh FederationsList.hh RTIG.hh	\
Trace.hh

rtig_SOURCES = Federate.cc Federation.cc FederationsList.cc main.cc	\
RTIG.cc RTIG_processing.cc Trace.cc

LDADD = -L$(top_builddir)/libCERTI -L$(top_builddir)/libpdebug -lCERTI	\
-lpdebug @ETR_SOCKET_LIBS@

## EOF $Id: Makefile.am,v 3.0 2002/11/21 01:27:51 breholee Exp $
