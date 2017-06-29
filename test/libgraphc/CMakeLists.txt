# FIXME I do not remember why this should be static?
# on non WINDOWS platform
set(BESTATIC "STATIC")
if (WIN32 AND MINGW)
    set(BESTATIC "STATIC")
endif(WIN32 AND MINGW)

add_library(graph-nogui ${BESTATIC} graph_c.cc)
set_target_properties(graph-nogui PROPERTIES COMPILE_FLAGS "-DX_DISPLAY_MISSING=1")


if(COMPILE_WITH_CXX11)
    if (CMAKE_VERSION VERSION_LESS 3.1.3)
        SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -std=c++11")
    else()
        set_property(TARGET graph-nogui PROPERTY CXX_STANDARD 11)
    endif()
endif()

install(TARGETS graph-nogui
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)

if (NOT X_DISPLAY_MISSING)
   include_directories(${X11_INCLUDE_DIR})
   add_library(graph ${BESTATIC} graph_c.cc)
   target_link_libraries(graph ${X11_LIBRARIES})

   if(COMPILE_WITH_CXX11)
      if (CMAKE_VERSION VERSION_LESS 3.1.3)
         SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -std=c++11")
      else()
         set_property(TARGET graph PROPERTY CXX_STANDARD 11)
      endif()
   endif()
   install(TARGETS graph
           RUNTIME DESTINATION bin
           LIBRARY DESTINATION lib
           ARCHIVE DESTINATION lib)
endif(NOT X_DISPLAY_MISSING)
