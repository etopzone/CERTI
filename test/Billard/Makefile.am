## $Id: Makefile.am,v 3.17 2007/07/25 14:35:08 erk Exp $
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
@ETR_SOCKET_LIBS@ @XML_LIBS@ @LEXLIB@ @POSIX_LIBS@

EXTRA_DIST = Test.fed Test.xml CMakeLists.txt
