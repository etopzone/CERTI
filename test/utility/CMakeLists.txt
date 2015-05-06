include_directories(
  ${CMAKE_SOURCE_DIR}/libCERTI 
  ${CMAKE_SOURCE_DIR}/libHLA
  )

find_library(RT_LIBRARY rt)
add_executable(CertiProcessus_A Main_SocketSHM.cc SharedStruct.hh)

target_link_libraries(CertiProcessus_A CERTI HLA)
if(RT_LIBRARY)
  target_link_libraries(CertiProcessus_A ${RT_LIBRARY})
endif()

set_target_properties(CertiProcessus_A PROPERTIES COMPILE_FLAGS -DSIDE_SC)

add_executable(CertiProcessus_B Main_SocketSHM.cc SharedStruct.hh)

target_link_libraries(CertiProcessus_B CERTI HLA)
if(RT_LIBRARY)
    target_link_libraries(CertiProcessus_B ${RT_LIBRARY})
endif()

set_target_properties(CertiProcessus_B PROPERTIES COMPILE_FLAGS -DSIDE_CS)

# Xml parsing test program
add_executable(CertiCheckXML checkXML.cc)
target_link_libraries(CertiCheckXML xml2 RTI)

install(TARGETS CertiProcessus_A CertiProcessus_B CertiCheckXML
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)


