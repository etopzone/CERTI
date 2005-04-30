## $Id: Makefile.am,v 3.6 2005/04/30 14:02:05 breholee Exp $
##
## Process this file with automake to produce Makefile.in

# @configure_input@

MAINTAINERCLEANFILES = Makefile.in

INCLUDES = -I$(top_srcdir)/include -I$(top_srcdir)/libCERTI @XML_CFLAGS@ @HAVE_XML@ @WITH_CYGWIN@

bin_PROGRAMS = rtig

noinst_HEADERS = Federate.hh Federation.hh FederationsList.hh RTIG.hh Trace.hh cmdline.h getopt.h

rtig_SOURCES = Federate.cc Federation.cc FederationsList.cc main.cc	\
RTIG.cc RTIG_processing.cc Trace.cc cmdline.c getopt1.c getopt.c

LDADD = -L$(top_builddir)/libCERTI -lCERTI @ETR_SOCKET_LIBS@ @XML_LIBS@ @LEXLIB@
