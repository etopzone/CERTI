enable_testing()

include_directories(${CERTI_SOURCE_DIR}) # include root to enable syntax #include <libHLA/...>
include_directories(${CERTI_BINARY_DIR})

find_package(Threads REQUIRED)

# Copy includes of the RTIG for now, but
# FIXME add only includes to source dir everywhere please...
include_directories(${CERTI_SOURCE_DIR}/libHLA)
include_directories(${CMAKE_SOURCE_DIR}/libCERTI)
include_directories(${CMAKE_SOURCE_DIR}/libRTI)

## Used for testing libcerti internal classes
set(lib_certi_SRCS
    ${CERTI_SOURCE_DIR}/libCERTI/ObjectClassBroadcastList.cc
    )

add_executable(TestLibCERTI
               
               ../mocks/sockettcp_mock.h
               
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

if (CMAKE_VERSION VERSION_LESS 3.1.3)
    include(CheckCXXCompilerFlag)
	check_cxx_compiler_flag(-std=c++14 HAVE_FLAG_STD_CXX14)
	if(HAVE_FLAG_STD_CXX14)
            SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -std=c++14")
	else()
            SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -std=c++1y")
	endif()
else()
    set_property(TARGET TestLibCERTI PROPERTY CXX_STANDARD 14)
endif()

add_test(AllTests TestLibCERTI)
