## CERTI rtig/Makefile.am 
## Copyright (C) 2001, 2002, 2003 ONERA
## $Id: Makefile.am,v 3.1 2003/01/14 17:17:33 breholee Exp $
##
## Process this file with automake to produce Makefile.in

# @configure_input@

MAINTAINERCLEANFILES = Makefile.in

INCLUDES = -I$(top_srcdir)/include -I$(top_srcdir)/libCERTI	\
-I$(top_srcdir)/libpdebug

bin_PROGRAMS = rtig

noinst_HEADERS = Federate.hh Federation.hh FederationsList.hh RTIG.hh	\
Trace.hh cmdline.h getopt.h

rtig_SOURCES = Federate.cc Federation.cc FederationsList.cc main.cc	\
RTIG.cc RTIG_processing.cc Trace.cc cmdline.c getopt1.c getopt.c

LDADD = -L$(top_builddir)/libCERTI -L$(top_builddir)/libpdebug -lCERTI	\
-lpdebug @ETR_SOCKET_LIBS@

## EOF $Id: Makefile.am,v 3.1 2003/01/14 17:17:33 breholee Exp $
