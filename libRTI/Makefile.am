## $Id: Makefile.am,v 3.5 2005/05/03 08:59:05 breholee Exp $
##
## Process this file with automake to produce Makefile.in

MAINTAINERCLEANFILES = Makefile.in

INCLUDES = -I$(top_srcdir)/include -I$(top_srcdir)/libCERTI @WITH_CYGWIN@

lib_LTLIBRARIES = libRTI.la

noinst_HEADERS = RTIambPrivateRefs.hh

libRTI_la_SOURCES = RTIambassador.cc RTIambPrivateRefs.cc

libRTI_la_LDFLAGS = -version-info 1:0:0
