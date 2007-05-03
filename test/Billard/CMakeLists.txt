INCLUDE_DIRECTORIES(
  ${CMAKE_SOURCE_DIR}/libCERTI 
  ${CMAKE_SOURCE_DIR}/test/libgraphc
  )


########### next target ###############

SET(billard_SRCS
  Billard.cc
  BillardDDM.cc
  Ball.cc
  ColoredBall.cc
  Display.cc
  main.cc
  cmdline.c
  getopt.c
  getopt1.c
  )

ADD_EXECUTABLE(billard ${billard_SRCS})

TARGET_LINK_LIBRARIES(billard ${SOCKET_LIBRARY} ${LIBXML2_LIBRARIES} graph CERTI RTI)

IF (X11_FOUND)
  TARGET_LINK_LIBRARIES(billard ${X11_LIBRARIES})
ENDIF (X11_FOUND)

INSTALL(TARGETS billard
  DESTINATION bin)


########### install files ###############

INSTALL(FILES  
  Test.fed Test.xml 
  DESTINATION share/federations)

#original Makefile.am contents follow:

### $Id: CMakeLists.txt,v 1.2 2007/05/03 08:24:01 erk Exp $
###
### Process this file with automake to produce Makefile.in
#
#feddir = $(prefix)/share/federations
#
#MAINTAINERCLEANFILES = Makefile.in
#
#INCLUDES = -I$(top_srcdir)/include -I$(top_srcdir)/libCERTI		\
#-I$(top_srcdir)/test/libgraphc @XML_CFLAGS@ @HAVE_XML@ @XCFLAGS@	\
#@WITH_X11@ @WITH_CYGWIN@
#
#bin_PROGRAMS = billard
#
#fed_DATA = Test.fed Test.xml
#
#noinst_HEADERS = Billard.hh BillardDDM.hh Ball.hh ColoredBall.hh	\
#Display.hh cmdline.h getopt.h
#
#billard_SOURCES = Billard.cc BillardDDM.cc Ball.cc ColoredBall.cc	\
#Display.cc main.cc cmdline.c getopt.c getopt1.c
#
#LDADD = -L$(top_builddir)/libRTI -L$(top_builddir)/test/libgraphc	\
#-L$(top_builddir)/libCERTI -lRTI -lgraph -lCERTI @XLIBS@		\
#@ETR_SOCKET_LIBS@ @XML_LIBS@ @LEXLIB@
#
#EXTRA_DIST = Test.fed Test.xml
