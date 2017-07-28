# FIXME I do not remember why this should be static?
# on non WINDOWS platform
set(BESTATIC "STATIC")
if (WIN32 AND MINGW)
    set(BESTATIC "STATIC")
endif(WIN32 AND MINGW)

add_library(graph-nogui ${BESTATIC} graph_c.cc)
set_target_properties(graph-nogui PROPERTIES COMPILE_FLAGS "-DX_DISPLAY_MISSING=1")


install(TARGETS graph-nogui
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)

if (NOT X_DISPLAY_MISSING)
   include_directories(${X11_INCLUDE_DIR})
   add_library(graph ${BESTATIC} graph_c.cc)
   target_link_libraries(graph ${X11_LIBRARIES})
   
   install(TARGETS graph
           RUNTIME DESTINATION bin
           LIBRARY DESTINATION lib
           ARCHIVE DESTINATION lib)
endif(NOT X_DISPLAY_MISSING)
