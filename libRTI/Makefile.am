## CERTI libRTI/Makefile.am 
## Copyright (C) 2001, 2002, 2003  ONERA
## $Id: Makefile.am,v 3.2 2003/07/01 13:26:16 breholee Exp $
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

## EOF $Id: Makefile.am,v 3.2 2003/07/01 13:26:16 breholee Exp $

