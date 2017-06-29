enable_testing()

include_directories(${CERTI_SOURCE_DIR}) # include root to enable syntax #include <libHLA/...>
include_directories(${CERTI_BINARY_DIR})

find_package(GTest REQUIRED)
find_package(Threads REQUIRED)

include_directories(${GTEST_INCLUDE_DIRS})

add_executable(TestHLA
               clock_test.cpp
               hlatypes_test.cpp
               messagebuffer_test.cpp
               msgbuffer_test.cpp
               )

target_link_libraries(TestHLA
                      HLA
                      ${GTEST_BOTH_LIBRARIES}
                      ${CMAKE_THREAD_LIBS_INIT}
                      )


if (CMAKE_VERSION VERSION_LESS 3.1.3)
    SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -std=c++11")
else()
    set_property(TARGET TestHLA PROPERTY CXX_STANDARD 11)
endif()

add_test(AllTests TestHLA)
