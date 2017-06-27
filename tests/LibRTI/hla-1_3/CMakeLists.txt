enable_testing()

include_directories(${CMAKE_SOURCE_DIR}/libRTI)
include_directories(${CMAKE_SOURCE_DIR}/include/hla-1_3)
include_directories(${CMAKE_BINARY_DIR}/include/hla-1_3)
include_directories(${CERTI_BINARY_DIR}) # for config.h

find_package(GTest REQUIRED)

include_directories(${GTEST_INCLUDE_DIRS})

add_executable(TestRTI-HLA-1_3
                fedtime_test.cpp
                )

target_link_libraries(TestRTI-HLA-1_3
                ${GTEST_BOTH_LIBRARIES}
                RTI)
                
set_property(TARGET TestRTI-HLA-1_3 PROPERTY CXX_STANDARD 11)

add_test(AllTests TestRTI-HLA-1_3)

