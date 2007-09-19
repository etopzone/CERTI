
#
# CMake include file CERTI usage helper
#
################ Socket library Check ####################
ENABLE_LANGUAGE(C)
# Load Checker macros
INCLUDE(CheckFunctionExists)
INCLUDE(CheckLibraryExists)
CHECK_LIBRARY_EXISTS("socket" "connect" "/usr/lib" SOCKET_LIBRARY_HAS_CONNECT)
IF (SOCKET_LIBRARY_HAS_CONNECT)
  MESSAGE(STATUS "Socket library needed for system ${CMAKE_SYSTEM_NAME}")
  SET(SOCKET_LIBRARY "socket")
ELSE (SOCKET_LIBRARY_HAS_CONNECT)
  SET(SOCKET_LIBRARY)
ENDIF (SOCKET_LIBRARY_HAS_CONNECT)

################ LIBXML install Check ####################
FIND_PACKAGE(LibXml2)
IF (NOT LIBXML2_FOUND)
  MESSAGE(STATUS "Libxml2 NOT found, won't be able to use .xml FOM files")
ENDIF(NOT LIBXML2_FOUND)

################ CERTI install Check ####################
FIND_PACKAGE(CERTI REQUIRED)

LINK_DIRECTORIES(${CERTI_LIBRARY_DIRS})
INCLUDE_DIRECTORIES(${CERTI_INCLUDE_DIRS})
ADD_DEFINITIONS(-DRTI_USES_STD_FSTREAM)
