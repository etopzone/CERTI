## $Id: Makefile.am,v 3.4 2005/04/30 17:41:59 breholee Exp $
##
## Process this file with automake to produce Makefile.in

MAINTAINERCLEANFILES = Makefile.in

INCLUDES = -I$(top_srcdir)/include -I$(top_srcdir)/libCERTI

lib_LTLIBRARIES = libRTI.la

noinst_HEADERS = RTIambPrivateRefs.hh

libRTI_la_SOURCES = RTIambassador.cc RTIambPrivateRefs.cc

# libRTI_la_LDFLAGS = -version-info 0:0:0
# too unstable to use version-info, so :
libRTI_la_LDFLAGS = -release 3.0.1
