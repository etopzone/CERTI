## CERTI RTIG Makefile.am 
## $Id: Makefile.am,v 3.4 2003/10/20 12:02:02 breholee Exp $
##
## Process this file with automake to produce Makefile.in

# @configure_input@

MAINTAINERCLEANFILES = Makefile.in

INCLUDES = -I$(top_srcdir)/include -I$(top_srcdir)/libCERTI	\
-I$(top_srcdir)/libpdebug @XML_CFLAGS@ @HAVE_XML@

bin_PROGRAMS = rtig

noinst_HEADERS = Federate.hh Federation.hh FederationsList.hh RTIG.hh	\
Trace.hh cmdline.h getopt.h

rtig_SOURCES = Federate.cc Federation.cc FederationsList.cc main.cc	\
RTIG.cc RTIG_processing.cc Trace.cc cmdline.c getopt1.c getopt.c

LDADD = -L$(top_builddir)/libCERTI -L$(top_builddir)/libpdebug -lCERTI	\
-lpdebug @ETR_SOCKET_LIBS@ @XML_LIBS@ @LEXLIB@

## EOF $Id: Makefile.am,v 3.4 2003/10/20 12:02:02 breholee Exp $
