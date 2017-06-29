add_subdirectory(libgraphc)
add_subdirectory(Billard)
add_subdirectory(utility)
add_subdirectory(testFederate)

# Do not compile this on Win32 (not very useful)
if (NOT WIN32)
   if (CMAKE_SYSTEM_NAME MATCHES "Linux")
      set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/certiCheckHostAndIP.c PROPERTIES COMPILE_DEFINITIONS "LINUX")
   endif()
   add_executable(CertiCheckHostAndIP certiCheckHostAndIP.c)
   if(COMPILE_WITH_C11)
      if (CMAKE_VERSION VERSION_LESS 3.1.3)
         SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -std=c++11")
      else()
         set_property(TARGET CertiCheckHostAndIP PROPERTY C_STANDARD 11)
      endif()
   endif()
   install(TARGETS CertiCheckHostAndIP
           RUNTIME DESTINATION bin
           LIBRARY DESTINATION lib
           ARCHIVE DESTINATION lib)
endif(NOT WIN32)
