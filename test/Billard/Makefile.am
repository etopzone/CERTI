## CERTI test/billard/Makefile.am
## Copyright (C) 2001, 2003 ONERA
## $Id: Makefile.am,v 3.5 2003/10/13 10:09:01 breholee Exp $
##
## Process this file with automake to produce Makefile.in

## @configure_input@

feddir = $(prefix)/share/federations

MAINTAINERCLEANFILES = Makefile.in

INCLUDES = -I$(top_srcdir)/include -I$(top_srcdir)/libpdebug	\
-I$(top_srcdir)/libCERTI -I$(top_srcdir)/test/include @XCFLAGS@	\
@WITH_X11@ @XML_CFLAGS@ @HAVE_XML@ -DTEST_USES_GRAPHICS

bin_PROGRAMS = billard
bin_SCRIPTS = billard-init billard-1 billard-2 billard-3

fed_DATA = Test.fed Test.xml

noinst_HEADERS = Billard.hh BillardDDM.hh Ball.hh ColoredBall.hh	\
Fed.hh Display.hh Objects.hh cmdline.h getopt.h

billard_SOURCES = Billard.cc BillardDDM.cc Ball.cc ColoredBall.cc	\
Fed.cc Display.cc Objects.cc main.cc cmdline.c getopt.c getopt1.c

LDADD = -L$(top_builddir)/libRTI -L$(top_builddir)/test/libgraphc	\
-L$(top_builddir)/libCERTI -L$(top_builddir)/libpdebug -lRTI -lgraph	\
-lCERTI -lpdebug @XLIBS@ @XML_LIBS@

EXTRA_DIST = Test.fed Test.xml billard-init billard-1 billard-2 billard-3

## EOF $Id: Makefile.am,v 3.5 2003/10/13 10:09:01 breholee Exp $
