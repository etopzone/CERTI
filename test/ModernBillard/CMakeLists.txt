include_directories(BEFORE
  ${CMAKE_SOURCE_DIR}/libCERTI 
  ${CMAKE_SOURCE_DIR}/test/libgraphc
  ${CMAKE_SOURCE_DIR}/include/
  ${CMAKE_SOURCE_DIR}/include/ieee1516-2010
  ${CMAKE_BINARY_DIR}/include/ieee1516-2010)
include_directories(${CERTI_SOURCE_DIR}/libHLA)

if (WIN32)
   add_definitions(-DHAVE_STRING_H)
endif (WIN32)

set(modern_billard_SRCS
    main.cc
    billard.cc
    )

if(X_DISPLAY_MISSING)
    message(STATUS "Some X11 header were not found: only Billard Test Application with no GUI will be built.")
else()
    add_executable(modern_billard ${modern_billard_SRCS})
    target_link_libraries(modern_billard
        graph RTI1516e FedTime1516e)  
    
    install(TARGETS modern_billard
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        )  
endif(X_DISPLAY_MISSING)

add_executable(modern_billard-nogui ${modern_billard_SRCS})

set_target_properties(modern_billard-nogui
    PROPERTIES COMPILE_FLAGS "-DX_DISPLAY_MISSING=1")
target_link_libraries(modern_billard-nogui
    RTI1516e FedTime1516e)

install(TARGETS modern_billard-nogui
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    )

if (CMAKE_VERSION VERSION_LESS 3.1.3)
    include(CheckCXXCompilerFlag)
    check_cxx_compiler_flag(-std=c++14 HAVE_FLAG_STD_CXX14)
    if(HAVE_FLAG_STD_CXX14)
        SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -std=c++14")
    else()
        SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -std=c++1y")
    endif()
else()
    if(TARGET modern_billard)
        set_property(TARGET modern_billard PROPERTY CXX_STANDARD 14)
    endif()
    
    set_property(TARGET modern_billard-nogui PROPERTY CXX_STANDARD 14)
endif()

install(FILES
    Base.xml Create.xml Join.xml 
    DESTINATION share/federations)
