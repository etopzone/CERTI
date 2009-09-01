INCLUDE_DIRECTORIES(
  ${CMAKE_SOURCE_DIR}/libCERTI 
  )

SET(CertiUtil_SRCS
  CertiUtilTests.cc 
  )

ADD_EXECUTABLE(CertiUtilTests ${CertiUtil_SRCS})

TARGET_LINK_LIBRARIES(CertiUtilTests CERTI)

INSTALL(TARGETS CertiUtilTests
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)
    
ADD_TEST(CertiUtilTests CertiUtilTests)

ADD_EXECUTABLE(CertiProcessus_A Main_SocketSHM_A.cc SharedStruct.hh)
TARGET_LINK_LIBRARIES(CertiProcessus_A CERTI rt)

ADD_EXECUTABLE(CertiProcessus_B Main_SocketSHM_B.cc SharedStruct.hh)
TARGET_LINK_LIBRARIES(CertiProcessus_B CERTI rt)

INSTALL(TARGETS CertiProcessus_A CertiProcessus_B
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)    