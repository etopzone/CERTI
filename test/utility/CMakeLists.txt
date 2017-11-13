include_directories(${CMAKE_SOURCE_DIR})

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
if(LIBXML2_FOUND)
   add_executable(CertiCheckXML checkXML.cc)
   target_link_libraries(CertiCheckXML ${LIBXML2_LIBRARIES} RTI)

   add_test(NAME CheckXML2000 COMMAND CertiCheckXML ${CMAKE_CURRENT_SOURCE_DIR}/T2000.xml)
   set_property(TEST CheckXML2000 PROPERTY PASS_REGULAR_EXPRESSION "2000")
   add_test(NAME CheckXML2010 COMMAND CertiCheckXML ${CMAKE_CURRENT_SOURCE_DIR}/T2010.xml)
   set_property(TEST CheckXML2010 PROPERTY PASS_REGULAR_EXPRESSION "2010")

   install(TARGETS CertiCheckXML
   RUNTIME DESTINATION bin
   LIBRARY DESTINATION lib
   ARCHIVE DESTINATION lib)
endif()

install(TARGETS CertiProcessus_A CertiProcessus_B
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)


