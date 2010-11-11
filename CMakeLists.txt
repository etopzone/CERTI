
PROJECT(CERTI CXX)

CMAKE_MINIMUM_REQUIRED(VERSION 2.4.4)
INCLUDE(${CMAKE_ROOT}/Modules/Dart.cmake)
MARK_AS_ADVANCED(TCL_TCLSH DART_ROOT)
# New Policy for full path library search
# see cmake --help-policy CMP0003
IF(COMMAND CMAKE_POLICY)
  CMAKE_POLICY(SET CMP0003 NEW)
ENDIF(COMMAND CMAKE_POLICY)
ENABLE_LANGUAGE(C)

# Use CERTI specific CMake modules first
SET(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/scripts)
#SET(CMAKE_VERBOSE_MAKEFILE ON)

# Load Checker macros
INCLUDE(CheckFunctionExists)
INCLUDE(CheckLibraryExists)

INCLUDE(TestBigEndian)
TEST_BIG_ENDIAN(BIG_ENDIAN)
IF(BIG_ENDIAN)
   ADD_DEFINITIONS(-DHOST_IS_BIG_ENDIAN)
ENDIF(BIG_ENDIAN)

# default behaviour is to build library as shared on all platform
OPTION(BUILD_SHARED
             "Build libraries as shared library" ON)

OPTION(USE_FULL_RPATH
             "Use the full RPATH" OFF)

IF (USE_FULL_RPATH)
    # see http://www.cmake.org/Wiki/CMake_RPATH_handling#Always_full_RPATH
    # use, i.e. don't skip the full RPATH for the build tree
    SET(CMAKE_SKIP_BUILD_RPATH  FALSE)

    # when building, don't use the install RPATH already
    # (but later on when installing)
    SET(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)

    # the RPATH to be used when installing
    SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")

    # add the automatically determined parts of the RPATH
    # which point to directories outside the build tree to the install RPATH
    SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
ENDIF(USE_FULL_RPATH)

OPTION(BUILD_LEGACY_LIBRTI
             "Build CERTI legacy libRTI" OFF)

OPTION(BUILD_DOC
             "Build documentation (using doxygen)" OFF)

OPTION(FORCE_NO_X11
              "Force not to use X11 (i.e. no Billard GUI)" OFF)

# On demand of Eric Noulard, the unsafe tsc clocksource
OPTION(ENABLE_TSC_CLOCK
             "Enable the usage of the timestamp counter as clock source (use with care)" OFF)

# The communication channel to RTIA
OPTION(RTIA_USE_TCP
           "Force TCP socket usage between RTIA and FederateAmbassador (default is Unix Socket)" OFF)
IF(RTIA_USE_TCP)
    ADD_DEFINITIONS(-DRTIA_USE_TCP)
ENDIF(RTIA_USE_TCP)

# The new NULL Prime message protocol
OPTION(CERTI_USE_NULL_PRIME_MESSAGE_PROTOCOL 
         "NULL PRIME MESSAGE protocol is an enhanced version of the CMB NULL MESSAGE protocol (experimental)" OFF)
IF(CERTI_USE_NULL_PRIME_MESSAGE_PROTOCOL)
    ADD_DEFINITIONS(-DCERTI_USE_NULL_PRIME_MESSAGE_PROTOCOL)
ENDIF(CERTI_USE_NULL_PRIME_MESSAGE_PROTOCOL)

# The CERTI Realtime extensions
OPTION(CERTI_REALTIME_EXTENSIONS
         "CERTI proposed realtime extension to HLA API" OFF)
IF(CERTI_REALTIME_EXTENSIONS)
    ADD_DEFINITIONS(-DCERTI_REALTIME_EXTENSIONS)
ENDIF(CERTI_REALTIME_EXTENSIONS)                  

IF (NOT CMAKE_BUILD_TYPE)
   MESSAGE(STATUS "No build type selected, default to Debug")
   SET(CMAKE_BUILD_TYPE "Debug")
ENDIF(NOT CMAKE_BUILD_TYPE)

SET(CMAKE_DEBUG_POSTFIX "d" CACHE STRING "debug library postfix, usually d on windows")

# Force CMAKE_COMPILER_IS_GNUCC even if gcc used with ccache
IF("${CMAKE_C_COMPILER}" MATCHES "ccache")
  IF("${CMAKE_C_COMPILER_ARG1}" MATCHES "gcc")
      MESSAGE(STATUS "Ok ccache with gcc forcing CMAKE_COMPILER_IS_GNUCC")
      SET(CMAKE_COMPILER_IS_GNUCC 1)
  ENDIF("${CMAKE_C_COMPILER_ARG1}" MATCHES "gcc")
ENDIF("${CMAKE_C_COMPILER}" MATCHES "ccache")

# Force CMAKE_COMPILER_IS_GNUCXX even if g++ used with ccache
IF("${CMAKE_CXX_COMPILER}" MATCHES "ccache")
  IF("${CMAKE_CXX_COMPILER_ARG1}" MATCHES "g\\+\\+")
      MESSAGE(STATUS "Ok ccache with gcc forcing CMAKE_COMPILER_IS_GNUCXX")
      SET(CMAKE_COMPILER_IS_GNUCXX 1)
  ENDIF("${CMAKE_CXX_COMPILER_ARG1}" MATCHES "g\\+\\+")
ENDIF("${CMAKE_CXX_COMPILER}" MATCHES "ccache")

# Use this in order to make Eclipse CDT parser handle error message smoothly
# see  http://www.cmake.org/Wiki/CMake:Eclipse
IF(CMAKE_COMPILER_IS_GNUCC)
  SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fmessage-length=0")
ENDIF(CMAKE_COMPILER_IS_GNUCC)

IF(CMAKE_COMPILER_IS_GNUCXX)
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fmessage-length=0")
ENDIF(CMAKE_COMPILER_IS_GNUCXX)

# Enforce strict ANSI C/C++ compliance checking
IF(NOT MINGW)
IF(CMAKE_COMPILER_IS_GNUCC)
  # we may add -Wextra if we want gcc to _really_ scream
  SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pedantic -ansi -Wall -Wno-unused-function")
ENDIF(CMAKE_COMPILER_IS_GNUCC)
IF(CMAKE_COMPILER_IS_GNUCXX)
  # we may add -Wextra if we want gcc to _really_ scream
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic -ansi -Wall -Woverloaded-virtual -Wno-unused-function")
ENDIF(CMAKE_COMPILER_IS_GNUCXX)
ENDIF(NOT MINGW)

IF (WIN32)
    IF (MINGW)
        SET(CMAKE_SHARED_LIBRARY_PREFIX "")
        SET(CMAKE_STATIC_LIBRARY_PREFIX "")
    ENDIF (MINGW)
	SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR})
	SET(LIBRARY_OUTPUT_PATH ${EXECUTABLE_OUTPUT_PATH})
ENDIF(WIN32)

IF (BUILD_SHARED)
  SET(BUILD_SHARED_LIBS ON)
ELSE (BUILD_SHARED)
  SET(BUILD_SHARED_LIBS OFF)
ENDIF (BUILD_SHARED)

SET(PACKAGE_NAME      "CERTI")
SET(PACKAGE_VERSION_MAJOR "3")
SET(PACKAGE_VERSION_MINOR "4")
SET(PACKAGE_VERSION_PATCH "0")
SET(PACKAGE_VERSION   "${PACKAGE_VERSION_MAJOR}.${PACKAGE_VERSION_MINOR}.${PACKAGE_VERSION_PATCH}")
# Autotools compatibility var
SET(VERSION ${PACKAGE_VERSION})
SET(PACKAGE_BUGREPORT "certi-devel@nongnu.org")

SET(PACKAGE_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})

MESSAGE(STATUS "Configuring ${PACKAGE_NAME} version ${PACKAGE_VERSION}")

# FIND PACKAGEs
################ Autotool HAVE_xxx compatibility #########
FIND_FILE(HAVE_DLFCN_H NAMES dlfcn.h)
FIND_FILE(HAVE_INTTYPES_H NAMES inttypes.h)
FIND_FILE(HAVE_MEMORY_H NAMES memory.h)
FIND_FILE(HAVE_STDINT_H NAMES stdint.h)
FIND_FILE(HAVE_STDLIB_H NAMES stdlib.h)
FIND_FILE(HAVE_STRING_H NAMES string.h)
FIND_FILE(HAVE_STRINGS_H NAMES strings.h)

FIND_FILE(HAVE_SYS_SELECT_H NAMES select.h
  PATH_SUFFIXES sys)
FIND_FILE(HAVE_SYS_SOCKET_H NAMES socket.h
  PATH_SUFFIXES sys)
FIND_FILE(HAVE_SYS_STAT_H NAMES stat.h
  PATH_SUFFIXES sys)
FIND_FILE(HAVE_SYS_TYPES_H NAMES types.h
  PATH_SUFFIXES sys)

FIND_FILE(HAVE_UNISTD_H NAMES unistd.h)

# FIXME find a way to do this properly
SET(SELECT_TYPE_ARG1 "int")
SET(SELECT_TYPE_ARG234 "(fd_set *)")
SET(SELECT_TYPE_ARG5 "(struct timeval *)")
SET(YYTEXT_POINTER 1)

SET(HAVE_NAMESPACES 1)
SET(HAVE_NUMERIC_LIMITS 1)
SET(STDC_HEADERS 1)
SET(ZERO "0")

IF (NOT WIN32)
################ Check for PosixClock Support ###########
INCLUDE(CheckIncludeFile)
CHECK_INCLUDE_FILE(time.h HAVE_TIME_H)
INCLUDE(CheckFunctionExists)
SET(CMAKE_REQUIRED_FLAGS "${CMAKE_REQUIRED_FLAGS}")
SET(CMAKE_REQUIRED_DEFINITIONS "${CMAKE_REQUIRED_DEFINITIONS}")
SET(CMAKE_REQUIRED_INCLUDES "${CMAKE_REQUIRED_INCLUDES}")
FIND_LIBRARY(RT_LIBRARY rt)
SET(CMAKE_REQUIRED_LIBRARIES ${CMAKE_REQUIRED_LIBRARIES} ${RT_LIBRARY})

CHECK_FUNCTION_EXISTS(clock_gettime HAVE_CLOCK_GETTIME)
CHECK_FUNCTION_EXISTS(clock_settime HAVE_CLOCK_SETTIME)
CHECK_FUNCTION_EXISTS(clock_getres HAVE_CLOCK_GETRES)
CHECK_FUNCTION_EXISTS(clock_nanosleep HAVE_CLOCK_NANOSLEEP)

IF (HAVE_CLOCK_GETTIME AND HAVE_CLOCK_SETTIME AND HAVE_CLOCK_GETRES)
    SET(HAVE_POSIX_CLOCK 1)
ENDIF (HAVE_CLOCK_GETTIME AND HAVE_CLOCK_SETTIME AND HAVE_CLOCK_GETRES)

################ Check for gettimeofday Support ###########
CHECK_INCLUDE_FILE(sys/time.h HAVE_SYS_TIME_H)
CHECK_FUNCTION_EXISTS(gettimeofday HAVE_GETTIMEOFDAY)

################ Check for TSCClock Support ###########
IF (ENABLE_TSC_CLOCK)
    SET(TSC_ENABLED_PROCESSOR_REGEX ".*x86_64.*|.*i686.*")
    MESSAGE(STATUS "CMAKE_SYSTEM_PROCESSOR=${CMAKE_SYSTEM_PROCESSOR}")
    IF (CMAKE_SYSTEM_PROCESSOR MATCHES "${TSC_ENABLED_PROCESSOR_REGEX}")
        SET(HAVE_TSC_CLOCK 1)
    ENDIF (CMAKE_SYSTEM_PROCESSOR MATCHES "${TSC_ENABLED_PROCESSOR_REGEX}")
ENDIF (ENABLE_TSC_CLOCK)

ENDIF (NOT WIN32)

ADD_DEFINITIONS(-DHAVE_CONFIG_H=1)

################## Gen library Check #####################
CHECK_LIBRARY_EXISTS("gen" "basename" "/usr/lib" GEN_LIBRARY_HAS_BASENAME)
IF (GEN_LIBRARY_HAS_BASENAME)
    MESSAGE(STATUS "Gen library needed for system ${CMAKE_SYSTEM_NAME}")
    FIND_LIBRARY(GEN_LIBRARY
        NAMES gen)
ELSE (GEN_LIBRARY_HAS_BASENAME)
    SET(GEN_LIBRARY)
ENDIF (GEN_LIBRARY_HAS_BASENAME)

################ Socket library Check ####################
CHECK_LIBRARY_EXISTS("socket" "connect" "/usr/lib" SOCKET_LIBRARY_HAS_CONNECT)
IF (SOCKET_LIBRARY_HAS_CONNECT)
    MESSAGE(STATUS "Socket library needed for system ${CMAKE_SYSTEM_NAME}")
    FIND_LIBRARY(SOCKET_LIBRARY
	                          NAMES socket)
ELSE (SOCKET_LIBRARY_HAS_CONNECT)
    SET(SOCKET_LIBRARY)
ENDIF (SOCKET_LIBRARY_HAS_CONNECT)

IF (WIN32)
    #FIND_LIBRARY(SOCKET_LIBRARY
	#                          NAMES Ws2_32
	#			DOC "The Winsock2 library")
	SET(SOCKET_LIBRARY Ws2_32)
	SET(HAVE_WIN_CLOCK 1)
ENDIF (WIN32)

################ LIBXML2 install Check ####################
FIND_PACKAGE(LibXml2)
IF (LIBXML2_FOUND)
    ADD_DEFINITIONS(-DHAVE_XML ${LIBXML2_DEFINITIONS})
    INCLUDE_DIRECTORIES(${LIBXML2_INCLUDE_DIR})
ELSE (LIBXML2_FOUND)
    SET(LIBXML2_LIBRARIES "")
ENDIF (LIBXML2_FOUND)

################ X11 install Check ####################
IF (NOT FORCE_NO_X11)
    FIND_PACKAGE(X11)
ELSE(NOT FORCE_NO_X11)
    MESSAGE(STATUS "FORCE_NO_X11 OPTION has been  set: Billard Test Application will not have GUI.")
    SET(X11_FOUND 0)
ENDIF(NOT FORCE_NO_X11)

IF (X11_FOUND)
  FIND_FILE(HAVE_X11_BITMAPS_CNTR_PTR
    NAMES cntr_ptr
    PATH_SUFFIXES X11/bitmaps
    DOC "X11 bitmaps header")

  IF(NOT HAVE_X11_BITMAPS_CNTR_PTR)
    SET(X_DISPLAY_MISSING 1)
    MESSAGE(STATUS "X11 header: X11/bitmaps/cntr_ptr not found.")
  ENDIF(NOT HAVE_X11_BITMAPS_CNTR_PTR)

  FIND_FILE(HAVE_X11_BITMAPS_DOT
    NAMES dot
    PATH_SUFFIXES X11/bitmaps
    DOC "X11 bitmaps header")

  IF(NOT HAVE_X11_BITMAPS_DOT)
    SET(X_DISPLAY_MISSING 1)
    MESSAGE(STATUS "X11 header: X11/bitmaps/dot not found.")
  ENDIF(NOT HAVE_X11_BITMAPS_DOT)

  FIND_FILE(HAVE_X11_BITMAPS_DROPBAR8
    NAMES dropbar8
    PATH_SUFFIXES X11/bitmaps
    DOC "X11 bitmaps header")

  IF(NOT HAVE_X11_BITMAPS_DROPBAR8)
    SET(X_DISPLAY_MISSING 1)
    MESSAGE(STATUS "X11 header: X11/bitmaps/dropbar8 not found.")
  ENDIF(NOT HAVE_X11_BITMAPS_DROPBAR8)

  FIND_FILE(HAVE_X11_BITMAPS_LEFT_PTR
    NAMES left_ptr
    PATH_SUFFIXES X11/bitmaps
    DOC "X11 bitmaps header")

  IF(NOT HAVE_X11_BITMAPS_LEFT_PTR)
    SET(X_DISPLAY_MISSING 1)
    MESSAGE(STATUS "X11 header: X11/bitmaps/left_ptr not found.")
  ENDIF(NOT HAVE_X11_BITMAPS_LEFT_PTR)

  FIND_FILE(HAVE_X11_BITMAPS_MENU16
    NAMES menu16
    PATH_SUFFIXES X11/bitmaps
    DOC "X11 bitmaps header")

  IF(NOT HAVE_X11_BITMAPS_MENU16)
    SET(X_DISPLAY_MISSING 1)
    MESSAGE(STATUS "X11 header: X11/bitmaps/menu16 not found.")
  ENDIF(NOT HAVE_X11_BITMAPS_MENU16)

  FIND_FILE(HAVE_X11_BITMAPS_RIGHT_PTR
    NAMES right_ptr
    PATH_SUFFIXES X11/bitmaps
    DOC "X11 bitmaps header")

  IF(NOT HAVE_X11_BITMAPS_RIGHT_PTR)
    SET(X_DISPLAY_MISSING 1)
    MESSAGE(STATUS "X11 header: X11/bitmaps/right_ptr not found.")
  ENDIF(NOT HAVE_X11_BITMAPS_RIGHT_PTR)

ELSE (X11_FOUND)
  SET(X_DISPLAY_MISSING 1)
ENDIF (X11_FOUND)

IF(X_DISPLAY_MISSING)
  MESSAGE(STATUS "Some X11 header were not found: Billard Test Application will not have GUI.")
ENDIF(X_DISPLAY_MISSING)

################ LEX YACC Check ####################
FIND_PACKAGE(LexYacc)

## now we may generate the Autotools compatible config.h
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/config.h.cmake
                            ${CMAKE_CURRENT_BINARY_DIR}/config.h )

# Globally used include dir
INCLUDE_DIRECTORIES(include)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR}/include)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})
# Globally used compiler flags
ADD_DEFINITIONS(-DRTI_USES_STD_FSTREAM)

ADD_SUBDIRECTORY( include )
ADD_SUBDIRECTORY( libHLA )
ADD_SUBDIRECTORY( libCERTI )
ADD_SUBDIRECTORY( RTIG )
ADD_SUBDIRECTORY( RTIA )
ADD_SUBDIRECTORY( libRTI )
ADD_SUBDIRECTORY( test )
ADD_SUBDIRECTORY( doc )
ADD_SUBDIRECTORY( scripts )

SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "CERTI An efficient Open Source HLA RunTime Infrastructure")
SET(CPACK_PACKAGE_VENDOR "ONERA/DTIM")
SET(CPACK_PACKAGE_CONTACT "certi-devel@nongnu.org")
SET(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/CERTI_DESCRIPTION.txt")
SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/COPYING")
SET(CPACK_PACKAGE_NAME "${PACKAGE_NAME}")
SET(CPACK_SYSTEM_NAME ${CMAKE_SYSTEM_NAME})

# Handle CPack-generator specific configuration
ADD_CUSTOM_TARGET(PACKAGE_TYPE
		  COMMAND ${CMAKE_COMMAND} -DSRCDIR="${CERTI_SOURCE_DIR}" -DDSTDIR="${CERTI_BINARY_DIR}" -DMSVC=${MSVC} -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} -DCMAKE_CFG_INTDIR=${CMAKE_CFG_INTDIR} -P ${CERTI_SOURCE_DIR}/scripts/GeneratePackageType.cmake)
		   
CONFIGURE_FILE("${CERTI_SOURCE_DIR}/CERTICPackOptions.cmake.in"
               "${CERTI_BINARY_DIR}/CERTICPackOptions.cmake" @ONLY)
SET(CPACK_PROJECT_CONFIG_FILE "${CERTI_BINARY_DIR}/CERTICPackOptions.cmake")

IF (WIN32)
    SET(CPACK_SOURCE_GENERATOR "ZIP")
    SET(CPACK_GENERATOR "NSIS;ZIP")
    ## Add a custom target callable from IDE (Mostly for Visual Studio)
    GET_FILENAME_COMPONENT(CPACK_COMMAND ${CMAKE_COMMAND} PATH)
    SET(CPACK_COMMAND ${CPACK_COMMAND}/cpack)
    ADD_CUSTOM_TARGET(PACKAGE_SOURCE_ZIP
              COMMAND ${CPACK_COMMAND} -G ZIP --config CPackSourceConfig.cmake
              COMMENTS "Build a ZIP file containing the source")

    # If this is a debug build
    # we should include debug runtime libs too
    SET(CMAKE_INSTALL_DEBUG_LIBRARIES "Yes")
    # Install Runtime Libraries
    INCLUDE(InstallRequiredSystemLibraries)
    
    # NSIS installer specific part
    SET(CPACK_PACKAGE_ICON "${CMAKE_SOURCE_DIR}/doc/\\\\certi.bmp")
    SET(CPACK_NSIS_CREATE_ICONS "
	      CreateShortCut \\\"$SMPROGRAMS\\\\$STARTMENU_FOLDER\\\\RTIG.lnk\\\" \\\"$INSTDIR\\\\bin\\\\RTIG.exe\\\"
	      CreateShortCut \\\"$SMPROGRAMS\\\\$STARTMENU_FOLDER\\\\Billard.lnk\\\" \\\"$INSTDIR\\\\bin\\\\billard.exe\\\" \\\"-n 1 -t 10 -FTest.fed -fTest\\\"
	      ")
    SET(CPACK_NSIS_DELETE_ICONS "
	      Delete \\\"$SMPROGRAMS\\\\$MUI_TEMP\\\\RTIG.lnk\\\"
	      Delete \\\"$SMPROGRAMS\\\\$MUI_TEMP\\\\Billard.lnk\\\"
	      ")
    SET(CPACK_NSIS_DISPLAY_NAME "${CPACK_PACKAGE_INSTALL_DIRECTORY} CERTI (HLA RTI)")
    SET(CPACK_NSIS_HELP_LINK "https:\\\\\\\\savannah.nongnu.org\\\\projects\\\\certi")
    SET(CPACK_NSIS_URL_INFO_ABOUT "http:\\\\\\\\savannah.nongnu.org\\\\projects\\\\certi")
    SET(CPACK_NSIS_CONTACT "certi-devel@nongnu.org")
    SET(CPACK_NSIS_MODIFY_PATH ON)

    SET(CPACK_NSIS_EXTRA_INSTALL_COMMANDS ${CPACK_NSIS_EXTRA_INSTALL_COMMANDS} "
		CopyFiles \\\"$INSTDIR\\\\bin\\\\CERTI.dll\\\" \\\"$SYSDIR\\\"
		CopyFiles \\\"$INSTDIR\\\\bin\\\\RTI.dll\\\" \\\"$SYSDIR\\\"
    ")


#	SET(CPACK_NSIS_EXTRA_INSTALL_COMMANDS ${CPACK_NSIS_EXTRA_INSTALL_COMMANDS} "
#		MessageBox MB_OK \\\"Copy Libraries to the system directory\\\"
#	")

	SET(CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS ${CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS} "
		Delete \\\"$SYSDIR\\\\CERTI.dll\\\"
		Delete \\\"$SYSDIR\\\\RTI.dll\\\"
	")

#	SET(CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS ${CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS} "
#		MessageBox MB_OK \\\"Delete Libraries from the system directory\\\"
#	")
ELSE (WIN32)
  SET(CPACK_RPM_PACKAGE_LICENSE "GPLv2 and LGPLv2")
  # Try to build a relocatable package (for CPackRPM)
  SET(CPACK_RPM_PACKAGE_RELOCATABLE TRUE)
  SET(CPACK_SOURCE_GENERATOR "TGZ;ZIP")
  SET(CPACK_GENERATOR "TGZ;RPM;DEB")
ENDIF (WIN32)

include(SystemSpecificInformations)

IF (SPECIFIC_COMPILER_NAME)
    SET(CPACK_SYSTEM_NAME ${SPECIFIC_SYSTEM_VERSION_NAME}-${SPECIFIC_COMPILER_NAME})
ELSE (SPECIFIC_COMPILER_NAME)
    SET(CPACK_SYSTEM_NAME ${SPECIFIC_SYSTEM_VERSION_NAME})
ENDIF(SPECIFIC_COMPILER_NAME)

# Avoid putting BUILDIR inside source package (simple case)
IF ("${CMAKE_BINARY_DIR}" STREQUAL "${CMAKE_SOURCE_DIR}")
     SET(CPACK_SOURCE_IGNORE_FILES "/\\\\.pydev.*;/\\\\.settings/;/CVS/;/\\\\.svn/;\\\\.swp$;\\\\.#;/#;.*~;cscope.*;\\\\.cproject;\\\\.project;autom4te.cache/*")
ELSE ("${CMAKE_BINARY_DIR}" STREQUAL "${CMAKE_SOURCE_DIR}")
     SET(CPACK_SOURCE_IGNORE_FILES "/\\\\.pydev.*;/\\\\.settings/;/CVS/;/\\\\.svn/;\\\\.swp$;\\\\.#;/#;.*~;cscope.*;\\\\.cproject;\\\\.project;autom4te.cache/*;${CMAKE_BINARY_DIR}/*")
ENDIF ("${CMAKE_BINARY_DIR}" STREQUAL "${CMAKE_SOURCE_DIR}")

SET(CPACK_PACKAGE_VERSION ${PACKAGE_VERSION})
SET(CPACK_PACKAGE_VERSION_MAJOR ${PACKAGE_VERSION_MAJOR})
SET(CPACK_PACKAGE_VERSION_MINOR ${PACKAGE_VERSION_MINOR})
SET(CPACK_PACKAGE_VERSION_PATCH ${PACKAGE_VERSION_PATCH})
#SET(CPACK_PACKAGE_CONTACT "certi-devel@nongnu.org")

INCLUDE(CPack)
INCLUDE(UsePackageBackup)

# The UseRPMTools and UseDebian thing
# are obsolete since CPack 2.6.0 which has
# builtin support for RPM and DEB.
# We may get rid of those sooner or later
INCLUDE(UseRPMTools)
IF(RPMTools_FOUND)
  RPMTools_ADD_RPM_TARGETS(certi)
ENDIF(RPMTools_FOUND)
INCLUDE(UseDebian)
IF (DEBIAN_FOUND)
  ADD_DEBIAN_TARGETS(certi)
ENDIF(DEBIAN_FOUND)

## Patch generating target
# The custom target is using a cmake script which in turn use execute_process,
# this way we may find a flexible way to build the patch
ADD_CUSTOM_TARGET(patch
                  COMMAND ${CMAKE_COMMAND} -DWDIR:PATH=${CMAKE_SOURCE_DIR} -DODIR:PATH=${CMAKE_BINARY_DIR} -P ${CMAKE_SOURCE_DIR}/scripts/CreatePatchCVS.cmake
                  COMMENT "Generating patch file ready for review at certi-devel@nongnu.org")

ENABLE_TESTING()

IF (WIN32)
  MESSAGE(STATUS "*** CERTI for WIN32 has been successfully configured ********")
ELSE (WIN32)
  MESSAGE(STATUS "*** CERTI has been successfully configured ********")
ENDIF (WIN32)
