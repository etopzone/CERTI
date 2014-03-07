# Include directories common to all standards
include_directories(${CMAKE_SOURCE_DIR}/libCERTI)  # for libCERTI :-)
include_directories(${CMAKE_SOURCE_DIR}/libHLA)    # for MessageBuffer
include_directories(${CMAKE_BINARY_DIR})           # for the config.h file
# Standard specific includes will then be added in the concerned directory

# Process standard specific libRTI implementation
add_subdirectory(hla-1_3)
add_subdirectory(ieee1516-2000)
add_subdirectory(ieee1516-2010)