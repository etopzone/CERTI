
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/libCERTI)

########### next target ###############

SET(RTI_LIB_SRCS
  RTIambassador.cc
  RTIambPrivateRefs.cc
  )

ADD_LIBRARY(RTI SHARED ${RTI_LIB_SRCS})

#SET_TARGET_PROPERTIES(RTI PROPERTIES VERSION 1.0.0 SOVERSION 0 )
INSTALL(TARGETS RTI 
  DESTINATION lib)


########### install files ###############


#original Makefile.am contents follow:

### $Id: CMakeLists.txt,v 1.1 2007/05/02 14:50:15 erk Exp $
###
### Process this file with automake to produce Makefile.in
#
#MAINTAINERCLEANFILES = Makefile.in
#
#INCLUDES = -I$(top_srcdir)/include -I$(top_srcdir)/libCERTI @WITH_CYGWIN@
#
#lib_LTLIBRARIES = libRTI.la
#
#noinst_HEADERS = RTIambPrivateRefs.hh
#
#libRTI_la_SOURCES = RTIambassador.cc RTIambPrivateRefs.cc
#
#libRTI_la_LDFLAGS = -version-info 1:0:0
