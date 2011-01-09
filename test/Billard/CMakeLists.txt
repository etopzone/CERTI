INCLUDE_DIRECTORIES(BEFORE
  ${CMAKE_SOURCE_DIR}/libCERTI 
  ${CMAKE_SOURCE_DIR}/test/libgraphc
  )
include_directories(${CERTI_SOURCE_DIR}/libHLA)

IF (WIN32)
   ADD_DEFINITIONS(-DHAVE_STRING_H)
ENDIF (WIN32)

########### next target ###############

SET(billard_SRCS
  Billard.cc
  BillardDDM.cc
  Ball.cc
  ColoredBall.cc
  Display.cc
  main.cc
  cmdline.c
  getopt.c
  getopt1.c
  )
  
IF(X_DISPLAY_MISSING)
  MESSAGE(STATUS "Some X11 header were not found: only Billard Test Application with no GUI will be built.")
ELSE(X_DISPLAY_MISSING)
  ADD_EXECUTABLE(billard ${billard_SRCS})
  TARGET_LINK_LIBRARIES(billard graph RTI FedTime)  
  TARGET_LINK_LIBRARIES(billard ${X11_LIBRARIES})
  INSTALL(TARGETS billard
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)  
ENDIF(X_DISPLAY_MISSING)

ADD_EXECUTABLE(billard-nogui ${billard_SRCS})
set_target_properties(billard-nogui PROPERTIES COMPILE_FLAGS "-DX_DISPLAY_MISSING=1")
TARGET_LINK_LIBRARIES(billard-nogui graph-nogui RTI FedTime)
INSTALL(TARGETS billard-nogui
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)  
    
########### install files ###############
INSTALL(FILES  
  Test.fed Test.xml 
  DESTINATION share/federations)
  
INSTALL(FILES  
  dtest_billard.py 
  PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE WORLD_READ WORLD_EXECUTE
  DESTINATION share/scripts)

