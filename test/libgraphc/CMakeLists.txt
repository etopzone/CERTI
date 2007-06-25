

ADD_LIBRARY(graph STATIC graph_c.cc)

########### install files ###############

INSTALL(TARGETS graph
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)

#original Makefile.am contents follow:

### $Id: CMakeLists.txt,v 1.3 2007/06/25 13:09:59 erk Exp $ 
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
### EOF $Id: CMakeLists.txt,v 1.3 2007/06/25 13:09:59 erk Exp $
