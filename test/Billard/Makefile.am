## CERTI test/billard/Makefile.am
## Copyright (C) 2001, 2003 ONERA
## $Id: Makefile.am,v 3.1 2003/01/20 17:45:49 breholee Exp $
##
## Process this file with automake to produce Makefile.in

# @configure_input@

feddir = $(prefix)/share/federations

MAINTAINERCLEANFILES = Makefile.in

INCLUDES = -I$(top_srcdir)/include -I$(top_srcdir)/libpdebug	\
-I$(top_srcdir)/libCERTI -I$(top_srcdir)/test/include @XCFLAGS@	\
@WITH_X11@ @XML_CFLAGS@ @HAVE_XML@

bin_PROGRAMS = billard
bin_SCRIPTS = billard-init billard-1 billard-2 billard-3

fed_DATA = Test.fed Test.xml

noinst_HEADERS = bille.hh constants.hh my_fed.hh
billard_SOURCES = bille.cc my_fed.cc billard.cc bille.hh constants.hh my_fed.hh

LDADD = -L$(top_builddir)/libRTI -L$(top_builddir)/test/libgraphc	\
-L$(top_builddir)/libCERTI -L$(top_builddir)/libpdebug -lRTI -lgraph	\
-lCERTI -lpdebug @XLIBS@ @XML_LIBS@

EXTRA_DIST = Test.fed Test.xml billard-init billard-1 billard-2 billard-3

## EOF $Id: Makefile.am,v 3.1 2003/01/20 17:45:49 breholee Exp $
