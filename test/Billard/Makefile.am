## $Id: Makefile.am,v 3.14 2005/04/30 14:30:30 breholee Exp $
##
## Process this file with automake to produce Makefile.in

feddir = $(prefix)/share/federations

MAINTAINERCLEANFILES = Makefile.in

INCLUDES = -I$(top_srcdir)/include -I$(top_srcdir)/libCERTI		\
-I$(top_srcdir)/test/libgraphc @XCFLAGS@ @WITH_X11@ @WITH_CYGWIN@

bin_PROGRAMS = billard

fed_DATA = Test.fed Test.xml

noinst_HEADERS = Billard.hh BillardDDM.hh Ball.hh ColoredBall.hh	\
Display.hh cmdline.h getopt.h

billard_SOURCES = Billard.cc BillardDDM.cc Ball.cc ColoredBall.cc	\
Display.cc main.cc cmdline.c getopt.c getopt1.c

LDADD = -L$(top_builddir)/libRTI -L$(top_builddir)/test/libgraphc	\
-L$(top_builddir)/libCERTI -lRTI -lgraph -lCERTI @XLIBS@

EXTRA_DIST = Test.fed Test.xml
