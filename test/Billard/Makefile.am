## CERTI test/billard/Makefile.am
## Copyright (C) 2001, 2003 ONERA
## $Id: Makefile.am,v 3.11 2003/12/02 16:51:34 breholee Exp $
##
## Process this file with automake to produce Makefile.in

## @configure_input@

feddir = $(prefix)/share/federations

MAINTAINERCLEANFILES = Makefile.in

INCLUDES = -I$(top_srcdir)/include -I$(top_srcdir)/libpdebug	\
-I$(top_srcdir)/libCERTI -I$(top_srcdir)/test/include @XCFLAGS@	\
@WITH_X11@ @XML_CFLAGS@ @HAVE_XML@ @WITH_CYGWIN@

bin_PROGRAMS = billard

fed_DATA = Test.fed Test.xml

noinst_HEADERS = Billard.hh Ball.hh ColoredBall.hh Display.hh	\
BillardNull.hh BillardDDM.hh Objects.hh cmdline.h	\
getopt.h

billard_SOURCES = Billard.cc Ball.cc ColoredBall.cc Display.cc		\
BillardNull.cc main.cc BillardDDM.cc Objects.cc cmdline.c	\
getopt.c getopt1.c

LDADD = -L$(top_builddir)/libRTI -L$(top_builddir)/test/libgraphc	\
-L$(top_builddir)/libCERTI -L$(top_builddir)/libpdebug -lRTI -lgraph	\
-lCERTI -lpdebug @XLIBS@ @XML_LIBS@

EXTRA_DIST = Test.fed Test.xml

## EOF $Id: Makefile.am,v 3.11 2003/12/02 16:51:34 breholee Exp $
