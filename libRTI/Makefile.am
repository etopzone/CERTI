## CERTI libRTI/Makefile.am 
## Copyright (C) 2001, 2002, 2003  ONERA
## $Id: Makefile.am,v 3.3 2003/07/04 12:00:58 breholee Exp $
##
## Process this file with automake to produce Makefile.in

# @configure_input@

MAINTAINERCLEANFILES = Makefile.in

INCLUDES = -I$(top_srcdir)/include -I$(top_srcdir)/libCERTI	\
-I$(top_srcdir)/libpdebug

lib_LTLIBRARIES = libRTI.la

libRTI_la_SOURCES = FederationServices.cc DeclarationServices.cc	\
ObjectServices.cc OwnershipServices.cc TimeServices.cc			\
DataDistributionServices.cc SupportServices.cc RTIambassador.cc

# libRTI_la_LDFLAGS = -version-info 0:0:0
# too unstable to use version-info, so :
libRTI_la_LDFLAGS = -release 3.0.1

## EOF $Id: Makefile.am,v 3.3 2003/07/04 12:00:58 breholee Exp $

