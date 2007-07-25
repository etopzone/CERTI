## $Id: Makefile.am,v 1.1 2007/07/25 14:35:06 erk Exp $
##
## Process this file with automake to produce Makefile.in

MAINTAINERCLEANFILES = Makefile.in

INCLUDES = -I$(top_srcdir)/include -I$(top_srcdir)/libCERTI		\
 @XML_CFLAGS@ @HAVE_XML@ @XCFLAGS@	\
 @WITH_CYGWIN@

bin_PROGRAMS = CertiUtilTests

CertiUtilTests_SOURCES = CertiUtilTests.cc

LDADD = -L$(top_builddir)/libCERTI -lCERTI 		\
@ETR_SOCKET_LIBS@ @XML_LIBS@ @LEXLIB@ @POSIX_LIBS@

EXTRA_DIST = CMakeLists.txt
