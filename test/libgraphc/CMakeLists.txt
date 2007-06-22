

ADD_LIBRARY(graph STATIC graph_c.cc)

########### install files ###############

INSTALL(TARGETS graph
  DESTINATION lib)

#original Makefile.am contents follow:

### $Id: CMakeLists.txt,v 1.2 2007/06/22 08:51:41 erk Exp $ 
#
#MAINTAINERCLEANFILES = Makefile.in
#
#INCLUDES = @XCFLAGS@ @WITH_X11@
#
#noinst_HEADERS = graph_c.hh
#
#lib_LIBRARIES = libgraph.a
#
#libgraph_a_SOURCES = graph_c.cc
#
#LDADD = @XLIBS@
#
### EOF $Id: CMakeLists.txt,v 1.2 2007/06/22 08:51:41 erk Exp $
