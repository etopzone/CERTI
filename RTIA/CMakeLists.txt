
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/libCERTI)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/libRTI)

########### next target ###############

SET(rtia_SRCS
  Communications.cc
  DataDistribution.cc
  DeclarationManagement.cc
  FederationManagement.cc
  Files.cc
  main.cc
  ObjectManagement.cc
  OwnershipManagement.cc
  RTIA.cc
  RTIA_federate.cc
  RTIA_network.cc
  Statistics.cc
  TimeManagement.cc
  )


ADD_EXECUTABLE(rtia ${rtia_SRCS})

TARGET_LINK_LIBRARIES(rtia ${SOCKET_LIBRARY} ${LIBXML2_LIBRARIES} CERTI RTI)

INSTALL(TARGETS rtia 
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)


########### install files ###############


#original Makefile.am contents follow:

### $Id: CMakeLists.txt,v 1.3 2007/06/25 13:09:58 erk Exp $
###
### Process this file with automake to produce Makefile.in
#
## @configure_input@
#
#MAINTAINERCLEANFILES = Makefile.in
#
#INCLUDES = -I$(top_srcdir)/include -I$(top_srcdir)/libCERTI	\
#@XML_CFLAGS@ @HAVE_XML@ @WITH_CYGWIN@
#
#bin_PROGRAMS = rtia
#
#noinst_HEADERS = Communications.hh DataDistribution.hh			\
#DeclarationManagement.hh FederationManagement.hh Files.hh		\
#ObjectManagement.hh OwnershipManagement.hh RTIA.hh Statistics.hh	\
#TimeManagement.hh
#
#rtia_SOURCES = Communications.cc DataDistribution.cc			\
#DeclarationManagement.cc FederationManagement.cc Files.cc main.cc	\
#ObjectManagement.cc OwnershipManagement.cc RTIA.cc RTIA_federate.cc	\
#RTIA_network.cc Statistics.cc TimeManagement.cc
#
#LDADD = -L$(top_builddir)/libCERTI -lCERTI @ETR_SOCKET_LIBS@	\
#@XML_LIBS@ @LEXLIB@
