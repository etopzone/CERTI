

ADD_LIBRARY(graph graph_c.cc)

########### install files ###############

INSTALL(TARGETS graph
  DESTINATION lib)

#original Makefile.am contents follow:

### $Id: CMakeLists.txt,v 1.1 2007/05/02 14:50:15 erk Exp $ 
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
### EOF $Id: CMakeLists.txt,v 1.1 2007/05/02 14:50:15 erk Exp $
