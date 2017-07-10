enable_testing()

include_directories(${CERTI_SOURCE_DIR}) # include root to enable syntax #include <libHLA/...>
include_directories(${CERTI_BINARY_DIR})

find_package(Threads REQUIRED)

# Copy includes of the RTIG for now, but
# FIXME add only includes to source dir everywhere please...
include_directories(${CERTI_SOURCE_DIR}/libHLA)
include_directories(${CMAKE_SOURCE_DIR}/libCERTI)
include_directories(${CMAKE_SOURCE_DIR}/libRTI)

set(rtig_SRCS
    ${CERTI_SOURCE_DIR}/RTIG/Federate.hh
    ${CERTI_SOURCE_DIR}/RTIG/Federate.cc
    
    ${CERTI_SOURCE_DIR}/RTIG/Federation.hh
    ${CERTI_SOURCE_DIR}/RTIG/Federation.cc
    
    ${CERTI_SOURCE_DIR}/RTIG/FederationsList.hh
    ${CERTI_SOURCE_DIR}/RTIG/FederationsList.cc
    
    ${CERTI_SOURCE_DIR}/RTIG/RTIG.hh
    ${CERTI_SOURCE_DIR}/RTIG/RTIG.cc
    ${CERTI_SOURCE_DIR}/RTIG/RTIG_processing.cc
    )

add_executable(TestRTIG
               federate_test.cpp
               ${rtig_SRCS}
               ../main.cpp
               )

target_link_libraries(TestRTIG
                      CERTI
                      ${GTEST_BOTH_LIBRARIES}
                      ${CMAKE_THREAD_LIBS_INIT}
                      )


if (CMAKE_VERSION VERSION_LESS 3.1.3)
    SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -std=c++11")
else()
    set_property(TARGET TestRTIG PROPERTY CXX_STANDARD 11)
endif()

add_test(AllTests TestRTIG)
