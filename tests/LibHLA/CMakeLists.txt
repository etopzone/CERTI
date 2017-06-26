enable_testing()

include_directories(${CERTI_SOURCE_DIR}) # include root to enable syntax #include <libHLA/...>
include_directories(${CERTI_BINARY_DIR})

find_package(GTest REQUIRED)

include_directories(${GTEST_INCLUDE_DIRS})

add_executable(TestHLA
                clock_test.cpp
                hlatypes_test.cpp
                messagebuffer_test.cpp
                msgbuffer_test.cpp
                )

target_link_libraries(TestHLA
                ${GTEST_BOTH_LIBRARIES}
                HLA)
                
set_property(TARGET TestHLA PROPERTY CXX_STANDARD 11)

add_test(AllTests TestHLA)
