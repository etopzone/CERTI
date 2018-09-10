cmake_minimum_required(VERSION 2.8.11)

project(CERTI CXX)

include(${CMAKE_ROOT}/Modules/Dart.cmake)
mark_as_advanced(TCL_TCLSH DART_ROOT)
# New Policy for full path library search
# see cmake --help-policy CMP0003
if(COMMAND CMAKE_POLICY)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND CMAKE_POLICY)
enable_language(C)

# Use CERTI specific CMake modules first
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/scripts)
#set(CMAKE_VERBOSE_MAKEFILE ON)

# Load Checker macros
include(CheckFunctionExists)
include(CheckLibraryExists)


# Display some information on host and target (when cross-compiling)
if (CMAKE_CROSSCOMPILING)
   message(STATUS "Cross-compiling build.")
else (CMAKE_CROSSCOMPILING)
   message(STATUS "Native build.")
endif (CMAKE_CROSSCOMPILING)

# Test 32/64 bits
if("${CMAKE_SIZEOF_VOID_P}" EQUAL "8")
   message(STATUS "Target is 64 bits")
   add_definitions(-DHOST_IS_64BITS)
   if (WIN32)
       set(WINXXBITS Win64)
   endif(WIN32)
else("${CMAKE_SIZEOF_VOID_P}" EQUAL "8")
   message(STATUS "Target is 32 bits")
   if (WIN32)
       set(WINXXBITS Win32)
   endif(WIN32)
endif("${CMAKE_SIZEOF_VOID_P}" EQUAL "8")

# Test Endianess
include(TestBigEndian)
TEST_BIG_ENDIAN(BIG_ENDIAN)
if(BIG_ENDIAN)
   message(STATUS "Target is Big Endian")
   add_definitions(-DHOST_IS_BIG_ENDIAN)
else(BIG_ENDIAN)
   message(STATUS "Target is Little Endian")
endif(BIG_ENDIAN)

# default behaviour is to build library as shared on all platform
option(BUILD_SHARED
       "Build libraries as shared library" ON)

option(USE_FULL_RPATH
       "Use the full RPATH" OFF)
if (USE_FULL_RPATH)
    # see http://www.cmake.org/Wiki/CMake_RPATH_handling#Always_full_RPATH
    # use, i.e. don't skip the full RPATH for the build tree
    set(CMAKE_SKIP_BUILD_RPATH  FALSE)

    # when building, don't use the install RPATH already
    # (but later on when installing)
    set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)

    # the RPATH to be used when installing
    set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")

    # add the automatically determined parts of the RPATH
    # which point to directories outside the build tree to the install RPATH
    set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
endif(USE_FULL_RPATH)

option(BUILD_LEGACY_LIBRTI
       "Build CERTI legacy libRTI" OFF)

if (NOT BUILD_LEGACY_LIBRTI AND WIN32)
   option(CERTI_RTING_DLL_USE_LIB_PREFIX
          "Use lib prefix for DLL (Windows Only)" 
          OFF)
endif()

option(COMPILE_WITH_CXX14
       "Compile code with C++14 feature enabled" ON)
if(COMPILE_WITH_CXX14)
    if (CMAKE_VERSION VERSION_LESS 3.1.3)
        include(CheckCXXCompilerFlag)
        check_cxx_compiler_flag(-std=c++14 HAVE_FLAG_STD_CXX14)
        if(HAVE_FLAG_STD_CXX14)
                set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -std=c++14")
        else()
                set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -std=c++1y")
        endif()
        
        set(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -std=c11")
    else()
		  # using Visual Studio C++
		if (MSVC)
			include(CheckCXXCompilerFlag)
			check_cxx_compiler_flag(-std=c++14 HAVE_FLAG_STD_CXX14)
			if(HAVE_FLAG_STD_CXX14)
					set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -std=c++14")
					 message(STATUS "MSVS Compiling with C++14 Flag")
			else()
					set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -std=c++1y")
					message(STATUS "MSVS Compiling with C++11 Flag")
			endif()
			set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS}")
		else()
			set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -pedantic -Wall -Wextra -Woverloaded-virtual -Wno-unused-function ")
		endif()
        set(CMAKE_CXX_STANDARD 14)
        set(CMAKE_C_STANDARD 11)
    endif()
endif()

option(COMPILE_WITH_C11
       "Compile code with C11 feature enabled" ON)

option(COMPILE_UNIT_TESTS
       "Compile unit tests" OFF)

option(COMPILE_WITH_COVERAGE
       "Compile unit tests with coverage flags" OFF)

option(BUILD_DOC
       "Build documentation (using doxygen)" OFF)

option(FORCE_NO_X11
       "Force not to use X11 (i.e. no Billard GUI)" OFF)

# On demand of Eric Noulard, the unsafe tsc clocksource
option(ENABLE_TSC_CLOCK
       "Enable the usage of the timestamp counter as clock source (use with care)" OFF)

# The communication channel to RTIA
option(RTIA_USE_TCP
           "Force TCP socket usage between RTIA and FederateAmbassador (default is Unix Socket)" OFF)
if(RTIA_USE_TCP)
    add_definitions(-DRTIA_USE_TCP)
endif(RTIA_USE_TCP)

if (win32)
   option(RTIA_CONSOLE_SHOW
          "Windows specific: if set to ON the RTIA console will be shown" OFF)
   if(RTIA_CONSOLE_SHOW)
     add_defintions(-DRTIA_CONSOLE_SHOW)
   endif(RTIA_CONSOLE_SHOW)
endif()

# Select/Poll/EPoll Methods selected under Linux
option(CERTI_RTIG_USE_SELECT "CERTI rtig process will use standard select(..) call under linux" ON)
option(CERTI_RTIG_USE_POLL "CERTI rtig process will use special poll(..) call under linux" OFF)
option(CERTI_RTIG_USE_EPOLL "CERTI rtig process will use special epoll(..) call under linux" OFF)
IF(CERTI_RTIG_USE_SELECT AND NOT CERTI_RTIG_USE_POLL AND NOT CERTI_RTIG_USE_EPOLL)
    add_definitions(-DCERTI_RTIG_USE_SELECT)
    MESSAGE(STATUS "CERTI configured with standard select(..) function for rtig (Linux only)")
ELSEIF(CERTI_RTIG_USE_POLL AND NOT CERTI_RTIG_USE_SELECT AND NOT CERTI_RTIG_USE_EPOLL)
    add_definitions(-DCERTI_RTIG_USE_POLL)
    MESSAGE(STATUS "CERTI configured with special poll(..) function for rtig (Linux only)")
ELSEIF(CERTI_RTIG_USE_EPOLL AND NOT CERTI_RTIG_USE_SELECT AND NOT CERTI_RTIG_USE_POLL)
    add_definitions(-DCERTI_RTIG_USE_EPOLL)
    MESSAGE(STATUS "CERTI configured with special epoll(..) function for rtig (Linux only)")
ELSE()
	add_definitions(-DCERTI_RTIG_USE_SELECT)
	MESSAGE(STATUS "** WARNING: CONFIGURATION ISSUE WITH CONCURRENT CERTI_RTIG_USE_* FLAG ENABLED ***")
	MESSAGE(STATUS "--> CERTI configured with standard select(..) function for rtig (Linux only)")
ENDIF()
# The new NULL Prime message protocol
option(CERTI_USE_NULL_PRIME_MESSAGE_PROTOCOL
         "NULL PRIME MESSAGE protocol is an enhanced version of the CMB NULL MESSAGE protocol (experimental)" OFF)
IF(CERTI_USE_NULL_PRIME_MESSAGE_PROTOCOL)
    add_definitions(-DCERTI_USE_NULL_PRIME_MESSAGE_PROTOCOL)
ENDIF(CERTI_USE_NULL_PRIME_MESSAGE_PROTOCOL)

# The CERTI Realtime extensions
option(CERTI_REALTIME_EXTENSIONS
         "CERTI proposed realtime extension to HLA API" OFF)
IF(CERTI_REALTIME_EXTENSIONS)
    add_definitions(-DCERTI_REALTIME_EXTENSIONS)
ENDIF(CERTI_REALTIME_EXTENSIONS)

IF (NOT CMAKE_BUILD_TYPE)
   MESSAGE(STATUS "No build type selected, default to Debug")
   SET(CMAKE_BUILD_TYPE "Debug")
ENDIF(NOT CMAKE_BUILD_TYPE)

SET(CMAKE_DEBUG_POSTFIX "d" CACHE STRING "debug library postfix, usually 'd' on windows")
# This has to be set manually because varying version of MinGW support or not support
# this particular command line option, the default is not to use it.
# If you want it swicth this option to ON. 
option(ENABLE_MINGW_AUTOIMPORT "will add -enable-auto-import to MinGW compiler option" OFF)

#
# The Clang Sanitizer are tools developped by Google in order to track
# various erros at runtime:
#  see presentation at CPP CON 2014 here: 
#   https://channel9.msdn.com/Events/CPP/C-PP-Con-2014/Sanitize-your-CPP-code
#   https://github.com/CppCon/CppCon2014
#  - Address : ASan, i.e. address sanitizer
#  - Memory  : MSan, i.e. memory sanitizer
#  - Thread  : TSan, i.e. thread sanitizer
# See Clang doc as well: http://clang.llvm.org/docs/SanitizerSpecialCaseList.html
# Do not forget to set 
#  MSAN_SYMBOLIZER_PATH / ASAN_SYMBOLIZER_PATH
# to point to llvm-symbolizer
# See example usage in
#  http://clang.llvm.org/docs/AddressSanitizer.html
#  http://clang.llvm.org/docs/MemorySanitizer.html
#
# You may have to maintain a blacklist of non-to-be-analyze files and/or functions
# in scripts/sanitize-blacklist.txt
#
option(ENABLE_CLANG_SANITIZER "Will compile with clang sanitizers" OFF)
if (ENABLE_CLANG_SANITIZER)
    option(ENABLE_CLANG_SANITIZER_ADDRESS "Enable Address sanitizer" ON)
    option(ENABLE_CLANG_SANITIZER_MEMORY "Enable Memory sanitizer" OFF)
    # Pre-process sanitizer blacklist
    configure_file(${CMAKE_SOURCE_DIR}/scripts/sanitize-blacklist.txt.in ${CMAKE_BINARY_DIR}/sanitize-blacklist.txt)
endif()

if("${CMAKE_C_COMPILER}" MATCHES "clang.*")
    if (ENABLE_CLANG_SANITIZER_ADDRESS)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address -fPIE -fno-omit-frame-pointer -fsanitize-blacklist=${CMAKE_BINARY_DIR}/sanitize-blacklist.txt")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pie -fsanitize=address")
    endif()
    if (ENABLE_CLANG_SANITIZER_MEMORY)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=memory -fsanitize-memory-track-origins -fPIE -fno-omit-frame-pointer -fsanitize-blacklist=${CMAKE_BINARY_DIR}/sanitize-blacklist.txt")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pie -fsanitize=memory")
    endif()
endif()

if("${CMAKE_CXX_COMPILER}" MATCHES "clang\\+\\+.*")
    if (ENABLE_CLANG_SANITIZER_ADDRESS)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fPIE -fno-omit-frame-pointer -fsanitize-blacklist=${CMAKE_BINARY_DIR}/sanitize-blacklist.txt")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pie -fsanitize=address")
    endif()
    if (ENABLE_CLANG_SANITIZER_MEMORY)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=memory -fsanitize-memory-track-origins -fPIE -fno-omit-frame-pointer -fsanitize-blacklist=${CMAKE_BINARY_DIR}/sanitize-blacklist.txt")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pie -fsanitize=memory")
    endif()
endif()

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
  IF(NOT MINGW)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fvisibility=hidden")
  ENDIF(NOT MINGW)
ENDIF(CMAKE_COMPILER_IS_GNUCC)

IF(CMAKE_COMPILER_IS_GNUCXX)
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fmessage-length=0")
  IF(NOT MINGW)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden")
  ENDIF(NOT MINGW)
ENDIF(CMAKE_COMPILER_IS_GNUCXX)

# Enforce strict ANSI C/C++ compliance checking
IF(NOT MINGW)
IF(CMAKE_COMPILER_IS_GNUCC)
  # we may add -Wextra if we want gcc to _really_ scream
  SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pedantic -ansi -Wall -Wno-unused-function")
ENDIF(CMAKE_COMPILER_IS_GNUCC)

IF(CMAKE_COMPILER_IS_GNUCXX)
## made obsolete by c++11/14 migration, TODO remove
#  set(CXXSTD "c++98")
#  
#  # we need gnu++98 for long long and uint64 definition on some architecture
#  # (e.g. Solaris 8 + gcc 3.4.x)
#  if (CMAKE_SYSTEM MATCHES "SunOS")
#     set(CXXSTD "gnu++98")
#  endif(CMAKE_SYSTEM MATCHES "SunOS")
#  
#  # we may add -Wextra if we want gcc to _really_ scream
#  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=${CXXSTD} -Wall -Woverloaded-virtual -Wno-unused-function")
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Woverloaded-virtual -Wno-unused-function")
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
SET(PACKAGE_VERSION_MAJOR "4")
SET(PACKAGE_VERSION_MINOR "0")
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
if(RT_LIBRARY)
  SET(CMAKE_REQUIRED_LIBRARIES ${CMAKE_REQUIRED_LIBRARIES} ${RT_LIBRARY})
endif(RT_LIBRARY)

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

add_definitions(-DHAVE_CONFIG_H=1)

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

if(WIN32)
    #FIND_LIBRARY(SOCKET_LIBRARY
    #                          NAMES Ws2_32
    #			DOC "The Winsock2 library")
    set(SOCKET_LIBRARY ws2_32)
    set(HAVE_WIN_CLOCK 1)
    # We may need to bump the limit of the number of socket descriptor
    # that may be selected on Windows
    # See: https://msdn.microsoft.com/en-us/library/windows/desktop/ms739169%28v=vs.85%29.aspx
    # Following error encountered by a user:
    #   http://lists.nongnu.org/archive/html/certi-devel/2014-10/msg00002.html
    set(CERTI_WINDOWS_FD_SETSIZE "128" CACHE STRING "Number of selectable socket (Windows only) [0-> default Winsock value]")
    set_property(CACHE CERTI_WINDOWS_FD_SETSIZE PROPERTY STRINGS 0 64 128 256)
endif(WIN32)

if (CERTI_WINDOWS_FD_SETSIZE)
  message(STATUS "CERTI on Windows: maximum number of selectable socket: ${CERTI_WINDOWS_FD_SETSIZE}")
  add_definitions(-DFD_SETSIZE=${CERTI_WINDOWS_FD_SETSIZE})
endif()

################ LIBXML2 install Check ####################
FIND_PACKAGE(LibXml2)
IF (LIBXML2_FOUND)
    add_definitions(-DHAVE_XML ${LIBXML2_DEFINITIONS})
    INCLUDE_DIRECTORIES(${LIBXML2_INCLUDE_DIR})
ELSE (LIBXML2_FOUND)
    SET(LIBXML2_LIBRARIES "")
ENDIF (LIBXML2_FOUND)

################ X11 install Check ####################
IF (NOT FORCE_NO_X11)
    FIND_PACKAGE(X11)
ELSE(NOT FORCE_NO_X11)
    MESSAGE(STATUS "FORCE_NO_X11 OPTION has been set: only Billard Test Application with no GUI will be built.")
    SET(X11_FOUND 0)
ENDIF(NOT FORCE_NO_X11)

IF (X11_FOUND)
   message(STATUS "X11 include directory: ${X11_INCLUDE_DIR}")
   FIND_FILE(HAVE_X11_XLIB
     NAMES Xlib.h
     PATH_SUFFIXES X11/
     DOC "X11 Xlib header")

  IF(NOT HAVE_X11_XLIB)
    SET(X_DISPLAY_MISSING 1)
    MESSAGE(STATUS "X11 header: X11/Xlib not found.")
  ENDIF(NOT HAVE_X11_XLIB)

# on fedora, we need the package : xorg-x11-xbitmaps
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

################ LEX YACC Check ####################
FIND_PACKAGE(LexYacc)

## now we may generate the Autotools compatible config.h
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/config.h.cmake
                            ${CMAKE_CURRENT_BINARY_DIR}/config.h )

# Globally used include dir
include_directories(include)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/include)
include_directories(${CMAKE_CURRENT_BINARY_DIR})
# Globally used compiler flags
add_definitions(-DRTI_USES_STD_FSTREAM)

# FIXME currently on MacOS we have a "undefined symbol issue"
# The toggle to linker flag is a "quick fix" to avoid that
# but this is not the end of the story we need to fix it for good.
if (APPLE)
   SET (CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS_INIT} -flat_namespace -undefined suppress"
        CACHE STRING "Flags used by the linker during the creation of dll's." FORCE)
   # module linker flags
   SET (CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS_INIT} -flat_namespace -undefined suppress"
        CACHE STRING "Flags used by the linker during the creation of modules." FORCE)

   #set(PROPERTY LINK_FLAGS_DEBUG "-flat_namespace -undefined suppress")
endif(APPLE)

# FIX for mingw for auto-import without -enable-auto-import warning
if (MINGW)
   if (ENABLE_MINGW_AUTOIMPORT)
      set(AUTOIMPORT_FLAGS "-enable-auto-import")
      set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${AUTOIMPORT_FLAGS}")
      set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS_INIT} ${AUTOIMPORT_FLAGS}"
            CACHE STRING "Flags used by the linker during the creation of dll's." FORCE)
      # module linker flags
      set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS_INIT} ${AUTOIMPORT_FLAGS}"
            CACHE STRING "Flags used by the linker during the creation of modules." FORCE)
   endif()
endif(MINGW)

add_subdirectory( include )
add_subdirectory( libHLA )
add_subdirectory( libCERTI )
add_subdirectory( RTIG )
add_subdirectory( RTIA )
add_subdirectory( libRTI )
add_subdirectory( test )
add_subdirectory( doc )
add_subdirectory( scripts )
add_subdirectory( xml )

if (COMPILE_UNIT_TESTS)
    if (COMPILE_WITH_COVERAGE)
        MESSAGE(STATUS "Compiling with code coverage flags")
        
        include (CodeCoverage)
        
        APPEND_COVERAGE_COMPILER_FLAGS()
        set (CMAKE_CXX_OUTPUT_EXTENSION_REPLACE 1)
        
        set (COVERAGE_EXCLUDES
             'test/*'
             'build.tests.gtest/*'
             '/usr/include/*' )
    endif()
    
    add_subdirectory( tests )
endif()

SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "CERTI An efficient Open Source HLA RunTime Infrastructure")
SET(CPACK_PACKAGE_VENDOR "ONERA/DTIM")
SET(CPACK_PACKAGE_CONTACT "CERTI Development Team <certi-devel@nongnu.org>")
SET(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/CERTI_DESCRIPTION.txt")
SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/COPYING.txt")
SET(CPACK_PACKAGE_NAME "${PACKAGE_NAME}")
SET(CPACK_SYSTEM_NAME ${CMAKE_SYSTEM_NAME})

# Handle CPack-generator specific configuration
ADD_CUSTOM_TARGET(PACKAGE_TYPE
       COMMAND ${CMAKE_COMMAND}
                 -DSRCDIR="${CERTI_SOURCE_DIR}"
                 -DDSTDIR="${CERTI_BINARY_DIR}"
                 -DMSVC=${MSVC} -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
                 -DCMAKE_CFG_INTDIR=${CMAKE_CFG_INTDIR}
                 -P ${CERTI_SOURCE_DIR}/scripts/GeneratePackageType.cmake)

CONFIGURE_FILE("${CERTI_SOURCE_DIR}/CERTICPackOptions.cmake.in"
               "${CERTI_BINARY_DIR}/CERTICPackOptions.cmake" @ONLY)
SET(CPACK_PROJECT_CONFIG_FILE "${CERTI_BINARY_DIR}/CERTICPackOptions.cmake")

IF (WIN32)
    set(CPACK_SOURCE_GENERATOR "ZIP")
    set(CPACK_GENERATOR "ZIP")

    # Add NSIS generator iff makensis is found on the system
    # see http://nsis.sourceforge.net
    find_program(MAKENSIS_EXECUTABLE
                 NAMES makensis
                 DOC "The NSIS package maker command")
    if (MAKENSIS_EXECUTABLE)
      list(APPEND CPACK_GENERATOR "NSIS")
    endif(MAKENSIS_EXECUTABLE)

    # Add WIX generator iff candle compiler is found on the system
    # See http://wixtoolset.org/
    find_program(CANDLE_EXECUTABLE
                 NAMES candle
		         PATHS "$ENV{WIX}/bin"
                 DOC "The WIX package compiler command")    
    if (CANDLE_EXECUTABLE)
		list(APPEND CPACK_GENERATOR "WIX")
		set(CPACK_WIX_UPGRADE_GUID "CE218AF5-CAC3-4A99-8CB5-98DFC182DC1B")
		set(CPACK_WIX_PRODUCT_ICON "${CMAKE_SOURCE_DIR}/doc/certi-favi.jpg")
		set(CPACK_WIX_UI_BANNER "${CMAKE_SOURCE_DIR}/doc/certi-installer-banner-493x58.jpg")
		set(CPACK_WIX_UI_DIALOG "${CMAKE_SOURCE_DIR}/doc/certi-ui-dialog-493x312.jpg")
    endif (CANDLE_EXECUTABLE)

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
	      CreateShortCut \\\"$SMPROGRAMS\\\\$STARTMENU_FOLDER\\\\Billard_1.lnk\\\" \\\"$INSTDIR\\\\bin\\\\billard-nogui.exe\\\" \\\"-n Billard1 -t 10 -FTest.fed -fTest\\\"
	      CreateShortCut \\\"$SMPROGRAMS\\\\$STARTMENU_FOLDER\\\\Billard_2.lnk\\\" \\\"$INSTDIR\\\\bin\\\\billard-nogui.exe\\\" \\\"-n Billard2 -t 10 -FTest.fed -fTest\\\"
	      ")
    SET(CPACK_NSIS_DELETE_ICONS "
	      Delete \\\"$SMPROGRAMS\\\\$MUI_TEMP\\\\RTIG.lnk\\\"
	      Delete \\\"$SMPROGRAMS\\\\$MUI_TEMP\\\\Billard_1.lnk\\\"
	      Delete \\\"$SMPROGRAMS\\\\$MUI_TEMP\\\\Billard_2.lnk\\\"
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
  IF(APPLE)
    SET(CPACK_GENERATOR "TGZ;PackageMaker")
  ELSE(APPLE)
    SET(CPACK_GENERATOR "TGZ;RPM;DEB")
  ENDIF(APPLE)
ENDIF (WIN32)

include(SystemSpecificInformations)

IF(SPECIFIC_COMPILER_NAME)
  SET(CPACK_SYSTEM_NAME ${SPECIFIC_SYSTEM_VERSION_NAME}-${SPECIFIC_COMPILER_NAME})
ELSE(SPECIFIC_COMPILER_NAME)
  SET(CPACK_SYSTEM_NAME ${SPECIFIC_SYSTEM_VERSION_NAME})
ENDIF(SPECIFIC_COMPILER_NAME)

# Avoid putting BUILDIR inside source package (simple case)
IF ("${CMAKE_BINARY_DIR}" STREQUAL "${CMAKE_SOURCE_DIR}")
   SET(CPACK_SOURCE_IGNORE_FILES "/\\\\.pydev.*;/\\\\.settings/;/CVS/;/\\\\.svn/;\\\\.swp$;\\\\.#;/#;.*~;cscope.*;\\\\.cproject;\\\\.project;autom4te.cache/*;\\\\.git")
ELSE ("${CMAKE_BINARY_DIR}" STREQUAL "${CMAKE_SOURCE_DIR}")
   SET(CPACK_SOURCE_IGNORE_FILES "/\\\\.pydev.*;/\\\\.settings/;/CVS/;/\\\\.svn/;\\\\.swp$;\\\\.#;/#;.*~;cscope.*;\\\\.cproject;\\\\.project;autom4te.cache/*;${CMAKE_BINARY_DIR}/*;\\\\.git")
ENDIF ("${CMAKE_BINARY_DIR}" STREQUAL "${CMAKE_SOURCE_DIR}")

SET(CPACK_PACKAGE_VERSION ${PACKAGE_VERSION})
SET(CPACK_PACKAGE_VERSION_MAJOR ${PACKAGE_VERSION_MAJOR})
SET(CPACK_PACKAGE_VERSION_MINOR ${PACKAGE_VERSION_MINOR})
SET(CPACK_PACKAGE_VERSION_PATCH ${PACKAGE_VERSION_PATCH})

INCLUDE(CPack)
INCLUDE(UsePackageBackup)

if (CMAKE_VERSION VERSION_LESS 2.6.0)
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
endif (CMAKE_VERSION VERSION_LESS 2.6.0)

## Patch generating target
# The custom target is using a cmake script which in turn use execute_process,
# this way we may find a flexible way to build the patch
ADD_CUSTOM_TARGET(patch
                  COMMAND ${CMAKE_COMMAND} -DWDIR:PATH=${CMAKE_SOURCE_DIR} -DODIR:PATH=${CMAKE_BINARY_DIR} -P ${CMAKE_SOURCE_DIR}/scripts/CreatePatchCVS.cmake
                  COMMENT "Generating patch file ready for review at certi-devel@nongnu.org")

ENABLE_TESTING()

## Here comes the CERTI export things ##
# Create a CERTIBuildTreeSettings.cmake file for the use from the build tree
configure_file(CERTIBuildTreeSettings.cmake.in
  "${PROJECT_BINARY_DIR}/CERTIBuildTreeSettings.cmake" @ONLY)

#file(RELATIVE_PATH CONF_REL_INCLUDE_DIR "${INSTALL_CMAKE_DIR}"
#   "${INSTALL_INCLUDE_DIR}")
set(CONF_REL_INCLUDE_DIR "../../include")
configure_file(CERTIConfig.cmake.in
  "${PROJECT_BINARY_DIR}/CERTIConfig.cmake" @ONLY)
configure_file(CERTIConfigVersion.cmake.in
  "${PROJECT_BINARY_DIR}/CERTIConfigVersion.cmake" @ONLY)

# Install the Config mode files
install(FILES
        ${PROJECT_BINARY_DIR}/CERTIConfig.cmake
        ${PROJECT_BINARY_DIR}/CERTIConfigVersion.cmake
        DESTINATION share/scripts)
        
install(FILES
        scripts/HLAstandardMIM.xml 
        DESTINATION share/federations)
  
# And the exported set
install(EXPORT CERTIDepends DESTINATION share/scripts)


IF (WIN32)
  MESSAGE(STATUS "*** CERTI for ${WINXXBITS} has been successfully configured ********")
ELSE (WIN32)
  MESSAGE(STATUS "*** CERTI has been successfully configured ********")
ENDIF (WIN32)

add_test(TestPackage ${CMAKE_BUILD_TOOL} package)

### Uninstall ###
configure_file("${CMAKE_HOME_DIRECTORY}/cmake_uninstall.cmake.in" "cmake_uninstall.cmake" IMMEDIATE @ONLY)
add_custom_target(uninstall "${CMAKE_COMMAND}" -P "cmake_uninstall.cmake")
