# Add standard specific include directory
include_directories(${CMAKE_SOURCE_DIR}/include/ieee1516-2000)
include_directories(${CMAKE_BINARY_DIR}/include/ieee1516-2000)

##########################################################
# VariableLengthData
# This STATIC library is necessary on Windows in order
# to avoid circular dependencies
# FIXME
#   may be we can use CMake OBJECT library instead
# FIXME
##########################################################
if (WIN32)
  set(VARDATA1516_SRCS
      RTIvariableLengthData.cpp
      RTIvariableLengthDataImplementation.h
      ${CMAKE_SOURCE_DIR}/include/ieee1516-2000/RTI/VariableLengthData.h
     )

  source_group("Source Files\\VarData" FILES ${VARDATA1516_SRCS})

  add_library(VarData1516 STATIC ${VARDATA1516_SRCS})
  target_link_libraries(VarData1516)
  set_target_properties(VarData1516 PROPERTIES PROJECT_LABEL LibVarData1516)
  set_target_properties(VarData1516 PROPERTIES COMPILE_FLAGS "-DRTI_DISABLE_WARNINGS -DSTATIC_RTI")
  # There shouldn't be any def file for STATIC library since this is not a DLL
  # Commenting out next line.
  #if (MINGW)
  #    set_target_properties(VarData1516 PROPERTIES LINK_FLAGS "-Wl,--output-def,${LIBRARY_OUTPUT_PATH}/libVarData1516.def")
  #    install(FILES ${LIBRARY_OUTPUT_PATH}/libVarData1516.def
  #            DESTINATION lib)
  #endif (MINGW)
  # There shouldn't be any SOVERSION as well
  #set_target_properties(VarData1516 PROPERTIES VERSION 1.0.0 SOVERSION 1)
  set(VarData1516Target "VarData1516")
else()
  set(VarData1516Target "")
endif (WIN32)

##########################
# RTI1516
##########################

set(RTI1516_LIB_SRCS
    RTIambassadorFactory.cpp
    Exception.cpp
    RTIvariableLengthData.cpp
    RTIvariableLengthDataImplementation.h
    RTIambassador.cpp
    RTIambassadorImplementation.cpp
    RTIambassadorImplementation.h
    RTIambPrivateRefs.cpp
    RTIambPrivateRefs.h
    Handle.cpp
    HandleImplementation.cpp
    HandleImplementation.h
    RTIfedAmbassador.cpp
    RTIHandleFactory.cpp
    RTIHandleFactory.h
   )

set(RTI1516_LIB_INCLUDE
    ${CMAKE_SOURCE_DIR}/include/ieee1516-2000/RTI/Enums.h
    ${CMAKE_SOURCE_DIR}/include/ieee1516-2000/RTI/Exception.h
    ${CMAKE_SOURCE_DIR}/include/ieee1516-2000/RTI/FederateAmbassador.h
    ${CMAKE_SOURCE_DIR}/include/ieee1516-2000/RTI/Handle.h
    ${CMAKE_SOURCE_DIR}/include/ieee1516-2000/RTI/LogicalTime.h
    ${CMAKE_SOURCE_DIR}/include/ieee1516-2000/RTI/LogicalTimeFactory.h
    ${CMAKE_SOURCE_DIR}/include/ieee1516-2000/RTI/LogicalTimeInterval.h
    ${CMAKE_SOURCE_DIR}/include/ieee1516-2000/RTI/NullFederateAmbassador.h
    ${CMAKE_SOURCE_DIR}/include/ieee1516-2000/RTI/RangeBounds.h
    ${CMAKE_SOURCE_DIR}/include/ieee1516-2000/RTI/RTI1516.h
    ${CMAKE_SOURCE_DIR}/include/ieee1516-2000/RTI/RTIambassador.h
    ${CMAKE_SOURCE_DIR}/include/ieee1516-2000/RTI/RTIambassadorFactory.h
    ${CMAKE_SOURCE_DIR}/include/ieee1516-2000/RTI/SpecificConfig.h
    ${CMAKE_SOURCE_DIR}/include/ieee1516-2000/RTI/Typedefs.h
    ${CMAKE_SOURCE_DIR}/include/ieee1516-2000/RTI/VariableLengthData.h
)

add_library(RTI1516 ${RTI1516_LIB_SRCS} ${RTI1516_LIB_INCLUDE})
# Incorrect line
#target_link_libraries(RTI1516 CERTI)
# Correct line
target_link_libraries(RTI1516 CERTI FedTime1516)
install(FILES RTI1516fedTime.h DESTINATION include/ieee1516-2010/RTI)
message(STATUS "libRTI variant: HLA 1516")
set_target_properties(RTI1516 PROPERTIES OUTPUT_NAME "RTI1516")
set_target_properties(RTI1516 PROPERTIES COMPILE_FLAGS "-DHLA13NG_LIBRTI -DRTI_DISABLE_WARNINGS -DBUILDING_RTI")
set_target_properties(RTI1516 PROPERTIES VERSION 1.0.0 SOVERSION 1)
if (MINGW)
    set_target_properties(RTI1516 PROPERTIES LINK_FLAGS "-Wl,--output-def,${LIBRARY_OUTPUT_PATH}/libRTI1516.def")
    install(FILES ${LIBRARY_OUTPUT_PATH}/libRTI1516.def DESTINATION lib)
endif (MINGW)


##########################################################
set(FEDTIME1516_SRCS
    ${CMAKE_SOURCE_DIR}/include/ieee1516-2000/RTI/certiLogicalTime.h
    ${CMAKE_SOURCE_DIR}/include/ieee1516-2000/RTI/certiLogicalTimeInterval.h
    ${CMAKE_SOURCE_DIR}/include/ieee1516-2000/RTI/certiLogicalTimeFactory.h
    RTI1516fedTime.h
    RTI1516fedTime.cpp
   )

if (BUILD_WITH_EXTRA_LOGICAL_TIME)
   list(APPEND FEDTIME1516_SRC
        LogicalTimeDouble.h
        LogicalTimeDouble.cpp
        )
endif(BUILD_WITH_EXTRA_LOGICAL_TIME)

source_group("Source Files\\FedTime" FILES ${FEDTIME1516_SRCS})

add_library(FedTime1516 ${FEDTIME1516_SRCS})
# Correct line
target_link_libraries(FedTime1516 ${VarData1516Target})
# Incorrect line
# (at least the dependency exists but we cannot specify that
#  because we would generate a circular deps RTI1516-->FedTime1516-->RTI1516)
#target_link_libraries(FedTime1516 RTI1516)

set_target_properties(FedTime1516 PROPERTIES PROJECT_LABEL LibFedTime1516)
set_target_properties(FedTime1516 PROPERTIES COMPILE_FLAGS "-DBUILDING_FEDTIME -DRTI_DISABLE_WARNINGS -DSTATIC_RTI")

if(MINGW)
    set_target_properties(FedTime1516 PROPERTIES LINK_FLAGS "-Wl,--output-def,${LIBRARY_OUTPUT_PATH}/libFedTime1516.def")
    install(FILES ${LIBRARY_OUTPUT_PATH}/libFedTime1516.def
            DESTINATION lib)
endif(MINGW)

set_target_properties(FedTime1516 PROPERTIES VERSION 1.0.0 SOVERSION 1)

set(TestFedTime1516_SRCS
  TestFedTime.cc
)

#ADD_EXECUTABLE(TestFedTime1516 ${TestFedTime1516_SRCS})

# TODO: Should link with RTI1516 lib, instead of base RTI lib
#target_link_libraries(TestFedTime1516 FedTime1516 RTI1516)

#ADD_TEST(TestLibFedTime1516 COMMAND $<TARGET_FILE:TestFedTime1516>)


# Install rules for both libRTI156 and libFedTime1516
install(TARGETS RTI1516 FedTime1516 ${VarData1516Target}
        EXPORT CERTIDepends
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)

# mimic install directory structure in the binary tree in order to ease the linking 
# of builtin test (i.e. testFederate)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/RTI1516fedTime.h ${CMAKE_BINARY_DIR}/include/ieee1516-2000/RTI/RTI1516fedTime.h COPYONLY)
install(FILES RTI1516fedTime.h DESTINATION include/ieee1516-2000/RTI)
