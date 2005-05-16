## $Id: Makefile.am,v 3.15 2005/05/16 20:23:55 breholee Exp $
##
## Process this file with automake to produce Makefile.in

feddir = $(prefix)/share/federations

MAINTAINERCLEANFILES = Makefile.in

INCLUDES = -I$(top_srcdir)/include -I$(top_srcdir)/libCERTI		\
-I$(top_srcdir)/test/libgraphc @XML_CFLAGS@ @HAVE_XML@ @XCFLAGS@	\
@WITH_X11@ @WITH_CYGWIN@

bin_PROGRAMS = billard

fed_DATA = Test.fed Test.xml

noinst_HEADERS = Billard.hh BillardDDM.hh Ball.hh ColoredBall.hh	\
Display.hh cmdline.h getopt.h

billard_SOURCES = Billard.cc BillardDDM.cc Ball.cc ColoredBall.cc	\
Display.cc main.cc cmdline.c getopt.c getopt1.c

LDADD = -L$(top_builddir)/libRTI -L$(top_builddir)/test/libgraphc	\
-L$(top_builddir)/libCERTI -lRTI -lgraph -lCERTI @XLIBS@		\
@ETR_SOCKET_LIBS@ @XML_LIBS@ @LEXLIB@

EXTRA_DIST = Test.fed Test.xml
