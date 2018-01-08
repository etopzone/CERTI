enable_testing()

include_directories(${CERTI_SOURCE_DIR}) # include root to enable syntax #include <libHLA/...>
include_directories(${CERTI_BINARY_DIR})

find_package(Threads REQUIRED)

# Copy includes of the RTIG for now, but
# FIXME add only includes to source dir everywhere please...
include_directories(${CERTI_SOURCE_DIR})

set(rtig_SRCS
    ${CERTI_SOURCE_DIR}/RTIG/Federate.hh
    ${CERTI_SOURCE_DIR}/RTIG/Federate.cc
    
    ${CERTI_SOURCE_DIR}/RTIG/Federation.hh
    ${CERTI_SOURCE_DIR}/RTIG/Federation_fom.cc
    ${CERTI_SOURCE_DIR}/RTIG/Federation.cc
    
    ${CERTI_SOURCE_DIR}/RTIG/FederationsList.hh
    ${CERTI_SOURCE_DIR}/RTIG/FederationsList.cc
    
    ${CERTI_SOURCE_DIR}/RTIG/MessageProcessor.hh
    ${CERTI_SOURCE_DIR}/RTIG/MessageProcessor.cc
    
    ${CERTI_SOURCE_DIR}/RTIG/Mom.hh
    ${CERTI_SOURCE_DIR}/RTIG/Mom.cc
    ${CERTI_SOURCE_DIR}/RTIG/Mom_interactions.cc
    ${CERTI_SOURCE_DIR}/RTIG/Mom_objects.cc
    
    ${CERTI_SOURCE_DIR}/RTIG/RTIG.hh
    ${CERTI_SOURCE_DIR}/RTIG/RTIG.cc
    )

add_executable(TestRTIG
               temporaryenvironmentlocation.cpp temporaryenvironmentlocation.h
               temporaryfedfile.cpp temporaryfedfile.h
               
               ../mocks/sockettcp_mock.h

               federate_test.cpp
               federation_test.cpp
               federationlist_test.cpp
               messageprocessor_test.cpp
               
               mom_test.cpp
               
               ${rtig_SRCS}
               ../main.cpp
               )

target_link_libraries(TestRTIG
                      CERTI
                      ${GTEST_BOTH_LIBRARIES}
                      ${GMOCK_BOTH_LIBRARIES}
                      ${CMAKE_THREAD_LIBS_INIT}
                      )
                      
target_compile_definitions(TestRTIG PRIVATE CERTI_TEST)

if (COMPILE_WITH_COVERAGE)
    SETUP_TARGET_FOR_COVERAGE(
        NAME TestRTIG_coverage
        EXECUTABLE TestRTIG --gtest_output=xml:../output/results-TestRTIG.xml
        DEPENDENCIES TestRTIG
    )

    SETUP_TARGET_FOR_COVERAGE_COBERTURA(
        NAME TestRTIG_cobertura
        EXECUTABLE TestRTIG --gtest_output=xml:../output/results-TestRTIG.xml
        DEPENDENCIES TestRTIG
    )
endif()

add_test(AllTests TestRTIG)
