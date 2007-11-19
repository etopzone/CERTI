
# FIXME I do not remember why this should be static?
# on non WINDOWS platform
IF (WIN32 AND MINGW)
    ADD_LIBRARY(graph graph_c.cc)
ELSE(WIN32 AND MINGW)
    ADD_LIBRARY(graph STATIC graph_c.cc)
ENDIF(WIN32 AND MINGW)

INSTALL(TARGETS graph
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)
