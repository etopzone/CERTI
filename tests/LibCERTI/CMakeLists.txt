enable_testing()

include_directories(${CERTI_SOURCE_DIR}) # include root to enable syntax #include <libHLA/...>
include_directories(${CERTI_BINARY_DIR})

find_package(Threads REQUIRED)

include_directories(${CERTI_SOURCE_DIR})

## Used for testing libcerti internal classes
set(lib_certi_SRCS
    ${CERTI_SOURCE_DIR}/libCERTI/ObjectClassBroadcastList.cc
    )

add_executable(TestLibCERTI
               
               ../mocks/sockettcp_mock.h
               
               auditline_test.cpp
               
               networkmessage_test.cpp
               
               socketserver_test.cpp
               
               objectclassbroadcastlist_test.cpp
               objectclassbroadcastlist_benchmark.cpp
               
               ${lib_certi_SRCS}
               ../main.cpp
               )

target_link_libraries(TestLibCERTI
                      CERTI
                      ${GTEST_BOTH_LIBRARIES}
                      ${GMOCK_BOTH_LIBRARIES}
                      ${CMAKE_THREAD_LIBS_INIT}
                      )
                      
target_compile_definitions(TestLibCERTI PRIVATE CERTI_TEST)

if (COMPILE_WITH_COVERAGE)
    SETUP_TARGET_FOR_COVERAGE(
        NAME TestLibCERTI_coverage
        EXECUTABLE TestLibCERTI --gtest_output=xml:../output/results-TestLibCERTI.xml
        DEPENDENCIES TestLibCERTI
    )

    SETUP_TARGET_FOR_COVERAGE_COBERTURA(
        NAME TestLibCERTI_cobertura
        EXECUTABLE TestLibCERTI --gtest_output=xml:../output/results-TestLibCERTI.xml
        DEPENDENCIES TestLibCERTI
    )
endif()

add_test(AllTests TestLibCERTI)
