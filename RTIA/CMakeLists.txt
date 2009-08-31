
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/libCERTI)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/libRTI)

SET(rtia_SRCS
  Communications.cc Communications.hh
  DataDistribution.cc DataDistribution.hh
  DeclarationManagement.cc DeclarationManagement.hh
  FederationManagement.cc FederationManagement.hh
  Files.cc Files.hh
  main.cc
  ObjectManagement.cc ObjectManagement.hh
  OwnershipManagement.cc OwnershipManagement.hh
  RTIA.cc RTIA.hh
  RTIA_federate.cc
  RTIA_network.cc
  RTIA_cmdline.c RTIA_cmdline.h
  Statistics.cc Statistics.hh
  TimeManagement.cc TimeManagement.hh  
  )

ADD_EXECUTABLE(rtia ${rtia_SRCS})
TARGET_LINK_LIBRARIES(rtia CERTI)

INSTALL(TARGETS rtia 
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)
