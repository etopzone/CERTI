include_directories(BEFORE
  ${CMAKE_SOURCE_DIR}/libCERTI 
  ${CMAKE_SOURCE_DIR}/test/libgraphc
  ${CMAKE_SOURCE_DIR}/include/
  ${CMAKE_SOURCE_DIR}/include/ieee1516-2010
  ${CMAKE_BINARY_DIR}/include/ieee1516-2010)
include_directories(${CERTI_SOURCE_DIR}/libHLA)

if (WIN32)
   add_definitions(-DHAVE_STRING_H)
endif (WIN32)

########### next target ###############

set(mom_explorer_SRCS
  main.cc
  momfederateambassador.cc
  )

add_executable(mom_explorer ${mom_explorer_SRCS})
target_link_libraries(mom_explorer RTI1516e FedTime1516e)

install(TARGETS mom_explorer
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)

