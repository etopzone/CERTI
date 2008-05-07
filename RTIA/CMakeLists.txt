
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/libCERTI)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/libRTI)

SET(rtia_SRCS
  Communications.cc
  DataDistribution.cc
  DeclarationManagement.cc
  FederationManagement.cc
  Files.cc
  main.cc
  ObjectManagement.cc
  OwnershipManagement.cc
  RTIA.cc
  RTIA_federate.cc
  RTIA_network.cc
  Statistics.cc
  TimeManagement.cc
  )

ADD_EXECUTABLE(rtia ${rtia_SRCS})

TARGET_LINK_LIBRARIES(rtia ${SOCKET_LIBRARY} ${LIBXML2_LIBRARIES} CERTI RTI)

INSTALL(TARGETS rtia 
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)