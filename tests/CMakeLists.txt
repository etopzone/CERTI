find_package(GTest)
find_package(GMock)

if(GTest_FOUND AND GMock_FOUND)
    message( STATUS "GoogleTest: Found on the build system")
    
    include_directories(${GTEST_INCLUDE_DIRS})
    
    include_directories(${GMOCK_INCLUDE_DIRS})

else()
    message( STATUS "GoogleTest: Not found on the build system, fetching and building locally")
    # Enable ExternalProject CMake module
    include(ExternalProject)

    # Download and install GoogleTest
    ExternalProject_Add(
        gtest
        URL https://github.com/google/googletest/archive/master.zip
        PREFIX ${CMAKE_CURRENT_BINARY_DIR}/gtest
        # Disable install step
        INSTALL_COMMAND ""
    )

    # Get GTest source and binary directories from CMake project
    ExternalProject_Get_Property(gtest source_dir binary_dir)

    # Create a libgtest target to be used as a dependency by test programs
    add_library(libgtest IMPORTED STATIC GLOBAL)
    add_dependencies(libgtest gtest)

    # Set libgtest properties
    set_target_properties(libgtest PROPERTIES
        "IMPORTED_LOCATION" "${binary_dir}/googlemock/gtest/libgtest.a"
        "IMPORTED_LINK_INTERFACE_LIBRARIES" "${CMAKE_THREAD_LIBS_INIT}"
    )

    # Create a libgmock target to be used as a dependency by test programs
    add_library(libgmock IMPORTED STATIC GLOBAL)
    add_dependencies(libgmock gtest)

    # Set libgmock properties
    set_target_properties(libgmock PROPERTIES
        "IMPORTED_LOCATION" "${binary_dir}/googlemock/libgmock.a"
        "IMPORTED_LINK_INTERFACE_LIBRARIES" "${CMAKE_THREAD_LIBS_INIT}"
    )

    # I couldn't make it work with INTERFACE_INCLUDE_DIRECTORIES
    include_directories("${source_dir}/googletest/include"
                        "${source_dir}/googlemock/include")

    set(GTEST_BOTH_LIBRARIES libgtest)
    set(GMOCK_BOTH_LIBRARIES libgmock)
endif()

add_subdirectory( LibHLA )
add_subdirectory( LibRTI/hla-1_3 )
#add_subdirectory( LibRTI/ieee1516-2000 )
add_subdirectory( LibCERTI )
add_subdirectory( RTIG )
