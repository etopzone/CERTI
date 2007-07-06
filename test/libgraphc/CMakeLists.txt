
# FIXME I do not remember why this should be static?
ADD_LIBRARY(graph STATIC graph_c.cc)

INSTALL(TARGETS graph
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)
