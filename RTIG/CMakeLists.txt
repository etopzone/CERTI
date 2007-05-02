INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/libCERTI)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/libRTI)
########### next target ###############

SET(rtig_SRCS
  Federate.cc
  Federation.cc
  FederationsList.cc
  main.cc
  RTIG.cc
  RTIG_processing.cc
  Trace.cc
  cmdline.c
  getopt1.c
  getopt.c
  )

ADD_EXECUTABLE(rtig ${rtig_SRCS})

TARGET_LINK_LIBRARIES(rtig ${SOCKET_LIBRARY} ${LIBXML2_LIBRARIES} CERTI)

INSTALL(TARGETS rtig 
  DESTINATION bin)

########### install files ###############

#original Makefile.am contents follow:

### $Id: CMakeLists.txt,v 1.1 2007/05/02 14:50:15 erk Exp $
###
### Process this file with automake to produce Makefile.in
#
## @configure_input@
#
#MAINTAINERCLEANFILES = Makefile.in
#
#INCLUDES = -I$(top_srcdir)/include -I$(top_srcdir)/libCERTI @XML_CFLAGS@ @HAVE_XML@ @WITH_CYGWIN@
#
#bin_PROGRAMS = rtig
#
#noinst_HEADERS = Federate.hh Federation.hh FederationsList.hh RTIG.hh Trace.hh cmdline.h getopt.h
#
#rtig_SOURCES = Federate.cc Federation.cc FederationsList.cc main.cc	\
#RTIG.cc RTIG_processing.cc Trace.cc cmdline.c getopt1.c getopt.c
#
#LDADD = -L$(top_builddir)/libCERTI -lCERTI @ETR_SOCKET_LIBS@ @XML_LIBS@ @LEXLIB@
