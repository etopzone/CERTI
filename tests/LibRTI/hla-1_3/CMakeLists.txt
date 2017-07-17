enable_testing()

include_directories(${CMAKE_SOURCE_DIR}/libRTI)
include_directories(${CMAKE_SOURCE_DIR}/include/hla-1_3)
include_directories(${CMAKE_BINARY_DIR}/include/hla-1_3)
include_directories(${CERTI_BINARY_DIR}) # for config.h

find_package(Threads REQUIRED)

add_executable(TestRTI-HLA-1_3
                fedtime_test.cpp
                ../../main.cpp
                )

target_link_libraries(TestRTI-HLA-1_3
                      RTI
                      ${GTEST_BOTH_LIBRARIES}
                      ${CMAKE_THREAD_LIBS_INIT}
                      )
                

if (COMPILE_WITH_COVERAGE)
    SETUP_TARGET_FOR_COVERAGE(
        NAME TestRTI-HLA-1_3_coverage
        EXECUTABLE TestRTI-HLA-1_3 -j ${PROCESSOR_COUNT}
        DEPENDENCIES TestRTI-HLA-1_3
    )

    SETUP_TARGET_FOR_COVERAGE_COBERTURA(
        NAME TestRTI-HLA-1_3_cobertura
        EXECUTABLE TestRTI-HLA-1_3 -j ${PROCESSOR_COUNT}
        DEPENDENCIES TestRTI-HLA-1_3
    )
endif()

if (CMAKE_VERSION VERSION_LESS 3.1.3)
    SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -std=c++11")
else()
    set_property(TARGET TestRTI-HLA-1_3 PROPERTY CXX_STANDARD 11)
endif()

add_test(AllTests TestRTI-HLA-1_3)

