## CERTI librti/Makefile.am 
## Copyright (C) 2001, 2002 Onera
## $Id: Makefile.am,v 3.0.2.1 2002/11/22 18:41:03 breholee Exp $
##
## Process this file with automake to produce Makefile.in

# @configure_input@

MAINTAINERCLEANFILES = Makefile.in

INCLUDES = -I$(top_srcdir)/include -I$(top_srcdir)/libCERTI	\
-I$(top_srcdir)/libpdebug

lib_LTLIBRARIES = libRTI.la

libRTI_la_SOURCES = RTIambassador.cc 

# libRTI_la_LDFLAGS = -version-info 0:0:0
# too unstable to use version-info, so :
libRTI_la_LDFLAGS = -release 3.0.1

## EOF $Id: Makefile.am,v 3.0.2.1 2002/11/22 18:41:03 breholee Exp $

