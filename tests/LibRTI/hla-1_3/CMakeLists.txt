enable_testing()

include_directories(${CERTI_SOURCE_DIR})
include_directories(${CERTI_BINARY_DIR})
include_directories(${CMAKE_SOURCE_DIR}/include/hla-1_3)
include_directories(${CMAKE_BINARY_DIR}/include/hla-1_3)

find_package(Threads REQUIRED)

add_executable(TestRTI-HLA-1_3
                fedtime_test.cpp
                ../../main.cpp
                )

target_link_libraries(TestRTI-HLA-1_3
                      RTI FedTime
                      ${GTEST_BOTH_LIBRARIES}
                      ${CMAKE_THREAD_LIBS_INIT}
                      )
                      
target_compile_definitions(TestRTI-HLA-1_3 PRIVATE CERTI_TEST)
                

if (COMPILE_WITH_COVERAGE)
    SETUP_TARGET_FOR_COVERAGE(
        NAME TestRTI-HLA-1_3_coverage
        EXECUTABLE TestRTI-HLA-1_3 --gtest_output=xml:../output/results-TestRTI-1_3.xml
        DEPENDENCIES TestRTI-HLA-1_3
    )

    SETUP_TARGET_FOR_COVERAGE_COBERTURA(
        NAME TestRTI-HLA-1_3_cobertura
        EXECUTABLE TestRTI-HLA-1_3 --gtest_output=xml:../output/results-TestRTI-1_3.xml
        DEPENDENCIES TestRTI-HLA-1_3
    )
endif()

add_test(AllTests TestRTI-HLA-1_3)

