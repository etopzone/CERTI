INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/libCERTI)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/libRTI)

SET(rtig_SRCS
  Federate.cc
  Federation.cc
  FederationsList.cc
  main.cc
  RTIG.cc
  RTIG_processing.cc
  Trace.cc
  RTIG_cmdline.c  
  )

ADD_EXECUTABLE(rtig ${rtig_SRCS})
TARGET_LINK_LIBRARIES(rtig CERTI RTI-NG FedTime)

INSTALL(TARGETS rtig 
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)
