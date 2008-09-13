# - Find HLA RTI libraries
# This module finds if any HLA RTI is installed and determines where the include
# files and libraries are.
# See http://en.wikipedia.org/wiki/Run-Time_Infrastructure_(simulation)
#
# This code sets the following variables:
#  RTI_INCLUDE_DIRS = the directory(ies) where RTI includes file are found
#  RTI_LIBRARIES = The libraries to link against to use RTI
#  RTI_DEFINITIONS = -DRTI_USES_STD_FSTREAM
#  RTI_LIBRARY_DIRS = Set of library directories
#  RTI_FOUND = Set to FALSE if any HLA RTI was not found

INCLUDE(CMakeFindFrameworks)

MACRO(MESSAGE_QUIETLY QUIET TYPE MSG)
  IF(NOT ${QUIET})
    MESSAGE(${TYPE} "${MSG}")
  ENDIF(NOT ${QUIET})
ENDMACRO(MESSAGE_QUIETLY QUIET TYPE MSG)

# Detect the CERTI installation
# CERTI is an Open Source HLA RunTime Infrastructure
# See https://savannah.nongnu.org/projects/certi or http://www.cert.fr/CERTI/
IF ("$ENV{CERTI_HOME}" STRGREATER "")
  FILE(TO_CMAKE_PATH "$ENV{CERTI_HOME}" CERTI_HOME)
  MESSAGE_QUIETLY(RTI_FIND_QUIETLY STATUS "Using environment defined CERTI_HOME: ${CERTI_HOME}")
ENDIF ("$ENV{CERTI_HOME}" STRGREATER "")

SET(RTI_DEFINITIONS "-DRTI_USES_STD_FSTREAM")

# note: the following list is ordered to find first the most recent version
SET(POSSIBLE_DIRS
  ${CERTI_HOME}
  "[HKEY_LOCAL_MACHINE\\SOFTWARE\\MAK Technologies\\MAK RTI 3.2 MSVC++ 8.0;Location]"
  "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\MAK RTI 3.2-win32-msvc++8.0;InstallLocation]"
  "[HKEY_LOCAL_MACHINE\\SOFTWARE\\MAK Technologies\\MAK RTI 2.2;Location]"
  "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\MAK RTI 2.2;InstallLocation]"
  /usr
  /usr/local)

SET(CMAKE_FIND_LIBRARY_PREFIXES "lib" "")

FIND_LIBRARY(RTI_LIBRARY
  NAMES RTI RTI-NG
  PATHS ${POSSIBLE_DIRS}
  PATH_SUFFIXES lib
  DOC "The RTI Library")

IF (RTI_LIBRARY)
  SET(RTI_LIBRARIES ${RTI_LIBRARY})
  MESSAGE_QUIETLY(RTI_FIND_QUIETLY STATUS "RTI library found: ${RTI_LIBRARY}")
  GET_FILENAME_COMPONENT(RTI_LIBRARY_DIRS ${RTI_LIBRARY} PATH)
ELSE (RTI_LIBRARY)
  MESSAGE_QUIETLY(RTI_FIND_QUIETLY STATUS "RTI library NOT found")
ENDIF (RTI_LIBRARY)

FIND_LIBRARY(FEDTIME_LIBRARY
  NAMES FedTime
  PATHS ${POSSIBLE_DIRS}
  PATH_SUFFIXES lib
  DOC "The FedTime Library")

IF (FEDTIME_LIBRARY)
  SET(RTI_LIBRARIES ${RTI_LIBRARIES} ${FEDTIME_LIBRARY})
  MESSAGE_QUIETLY(RTI_FIND_QUIETLY STATUS "RTI FedTime found: ${FEDTIME_LIBRARY}")
ENDIF (FEDTIME_LIBRARY)

FIND_FILE(RTI_INCLUDE
  NAMES RTI.hh
  PATHS ${POSSIBLE_DIRS}
  PATH_SUFFIXES include
  DOC "The RTI Include Files")

IF (RTI_INCLUDE)
  MESSAGE_QUIETLY(RTI_FIND_QUIETLY STATUS "RTI headers found: ${RTI_INCLUDE}")
  GET_FILENAME_COMPONENT(RTI_INCLUDE_DIRS ${RTI_INCLUDE} PATH)
ELSE (RTI_INCLUDE)
  MESSAGE_QUIETLY(RTI_FIND_QUIETLY STATUS "RTI headers NOT found")
ENDIF (RTI_INCLUDE)

IF (RTI_LIBRARY AND RTI_INCLUDE)
  SET(RTI_FOUND TRUE)
ELSE (RTI_LIBRARY AND RTI_INCLUDE)
  SET(RTI_FOUND FALSE)
  IF (RTI_FIND_REQUIRED)
    MESSAGE(FATAL_ERROR "No RTI found! Please install a HLA RTI and re-run.")
  ENDIF (RTI_FIND_REQUIRED)
ENDIF(RTI_LIBRARY AND RTI_INCLUDE)

MARK_AS_ADVANCED(
  RTI_LIBRARIES
  RTI_INCLUDE_DIRS
  RTI_DEFINITIONS
  RTI_LIBRARY_DIRS)

# $Id: FindRTI.cmake,v 1.1 2008/09/13 18:59:49 gotthardp Exp $
