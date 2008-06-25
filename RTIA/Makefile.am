## $Id: Makefile.am,v 3.9 2008/06/25 08:28:12 erk Exp $
##
## Process this file with automake to produce Makefile.in

# @configure_input@

MAINTAINERCLEANFILES = Makefile.in

INCLUDES = -I$(top_srcdir)/include -I$(top_srcdir)/libCERTI	\
@XML_CFLAGS@ @HAVE_XML@ @WITH_CYGWIN@

bin_PROGRAMS = rtia

noinst_HEADERS = Communications.hh DataDistribution.hh			\
DeclarationManagement.hh FederationManagement.hh Files.hh		\
ObjectManagement.hh OwnershipManagement.hh RTIA.hh Statistics.hh	\
TimeManagement.hh RTIA_cmdline.h

rtia_SOURCES = Communications.cc DataDistribution.cc			\
DeclarationManagement.cc FederationManagement.cc Files.cc main.cc	\
ObjectManagement.cc OwnershipManagement.cc RTIA.cc RTIA_federate.cc	\
RTIA_network.cc Statistics.cc TimeManagement.cc RTIA_cmdline.c

LDADD = -L$(top_builddir)/libCERTI -lCERTI @ETR_SOCKET_LIBS@	\
@XML_LIBS@ @LEXLIB@ @POSIX_LIBS@

EXTRA_DIST = CMakeLists.txt RTIA.ggo
