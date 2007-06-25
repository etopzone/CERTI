
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/libCERTI)

########### next target ###############

SET(RTI_LIB_SRCS
  RTIambassador.cc
  RTIambPrivateRefs.cc
  )
  
SET(RTI_LIB_INCLUDE
../include/baseTypes.hh
../include/certi.hh
../include/Certi_Win.h
../include/federateAmbServices.hh
../include/NullFederateAmbassador.hh
../include/RTI.hh
../include/RTIambServices.hh
../include/RTItypes.hh
)

#SOURCE_GROUP("RTI\\Header" FILES ${RTI_LIB_INCLUDE})

ADD_LIBRARY(RTI SHARED ${RTI_LIB_SRCS} ${RTI_LIB_INCLUDE})
TARGET_LINK_LIBRARIES(RTI CERTI)

#SET_TARGET_PROPERTIES(RTI PROPERTIES VERSION 1.0.0 SOVERSION 0 )
INSTALL(TARGETS RTI 
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)

########### install files ###############


#original Makefile.am contents follow:

### $Id: CMakeLists.txt,v 1.3 2007/06/25 13:09:58 erk Exp $
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
