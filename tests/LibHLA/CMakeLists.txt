enable_testing()

include_directories(${CERTI_SOURCE_DIR}) # include root to enable syntax #include <libHLA/...>
include_directories(${CERTI_BINARY_DIR})

find_package(Threads REQUIRED)

add_executable(TestHLA
               clock_test.cpp
               hlatypes_test.cpp
               messagebuffer_test.cpp
               msgbuffer_test.cpp
               ../main.cpp
               )

target_link_libraries(TestHLA
                      HLA
                      ${GTEST_BOTH_LIBRARIES}
                      ${CMAKE_THREAD_LIBS_INIT}
                      )
                      
target_compile_definitions(TestHLA PRIVATE CERTI_TEST)

if (COMPILE_WITH_COVERAGE)
    SETUP_TARGET_FOR_COVERAGE(
        NAME TestHLA_coverage
        EXECUTABLE TestHLA --gtest_output=xml:../output/results-TestHLA.xml
        DEPENDENCIES TestHLA
    )

    SETUP_TARGET_FOR_COVERAGE_COBERTURA(
        NAME TestHLA_cobertura
        EXECUTABLE TestHLA --gtest_output=xml:../output/results-TestHLA.xml
        DEPENDENCIES TestHLA
    )
endif()

add_test(AllTests TestHLA)
