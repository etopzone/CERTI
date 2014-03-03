# Add standard specific include directory
include_directories(${CMAKE_SOURCE_DIR}/include/ieee1516-2010)
include_directories(${CMAKE_BINARY_DIR}/include/ieee1516-2010)

# TO BE CONTINUED

# mimic install directory structure in the binary tree in order to ease the linking 
# of builtin test (i.e. testFederate)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/RTI1516fedTime.h ${CMAKE_BINARY_DIR}/include/ieee1516-2010/RTI/RTI1516fedTime.h COPYONLY)
install(FILES RTI1516fedTime.h DESTINATION include/ieee1516-2010/RTI)