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

########### next target ###############

set(modern_billard_SRCS
  main.cc
  billard.cc
  )

if(COMPILE_WITH_CXX11)
    if (CMAKE_VERSION VERSION_LESS 3.1.3)
	include(CheckCXXCompilerFlag)
	check_cxx_compiler_flag(-std=c++14 HAVE_FLAG_STD_CXX14)
	if(HAVE_FLAG_STD_CXX14)
            SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -std=c++14")
	else()
            SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -std=c++1y")
	endif()
    else()
        set_property(TARGET modern_billard PROPERTY CXX_STANDARD 14)
    endif()
endif()

add_executable(modern_billard ${modern_billard_SRCS})
target_link_libraries(modern_billard graph RTI1516e FedTime1516e)

install(FILES
  Base.xml Create.xml Join.xml 
  DESTINATION share/federations)

install(TARGETS modern_billard
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)
