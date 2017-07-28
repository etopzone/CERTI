set(LIBHLA_EXPORTED_INCLUDES libhla.hh)

set(LIBHLA_TYPES1516_SRCS
    HLAbuffer.hh HLAbuffer.cc
)
list(APPEND LIBHLA_EXPORTED_INCLUDES 
    HLAtypesIEEE1516.hh
    HLAbuffer.hh
    HLAbasicType.hh
    HLAenumeratedType.hh
    HLAfixedArray.hh
    HLAvariableArray.hh
    HLAfixedRecord.hh
    HLAvariantRecord.hh)
source_group("Source Files\\Types1516" FILES ${LIBHLA_TYPES1516_SRCS})

enable_language(C)
set(LIBHLA_HASH_SRCS sha1.c MurmurHash2.cpp MurmurHash3.cpp PMurHash.c)
list(APPEND LIBHLA_EXPORTED_INCLUDES sha1.h MurmurHash2.h MurmurHash3.h PMurHash.h)
set_source_files_properties(sha1.c PMurHash.c PROPERTIES LANGUAGE "C")
source_group("Source Files\\Hash" FILES ${LIBHLA_HASH_SRCS})

# Currently TLSF does not compile as-is on WIN32
if (NOT WIN32)
  set(LIBHLA_ALLOC_SRCS tlsf.c tlsf_target.h)
  list(APPEND LIBHLA_EXPORTED_INCLUDES tlsf.h)
  set_source_files_properties(tlsf.c PROPERTIES LANGUAGE "C")
  set_source_files_properties(tlsf.c PROPERTIES CFLAGS "-Wextra -Wall -Wwrite-strings -Wstrict-prototypes -Wmissing-prototypes -Wno-long-long -Wstrict-aliasing=2")
  source_group("Source Files\\Alloc" FILES ${LIBHLA_ALLOC_SRCS} tlsf.h)
endif (NOT WIN32)

set(LIBHLA_MB_SRCS MessageBuffer.cc MsgBuffer.c)
list(APPEND LIBHLA_EXPORTED_INCLUDES MessageBuffer.hh MsgBuffer.h)
set_source_files_properties(MsgBuffer.c PROPERTIES LANGUAGE "C")
if (MSVC)
	set_source_files_properties(MsgBuffer.c PROPERTIES CFLAGS "-D_CRT_SECURE_NO_WARNINGS")
endif ()

set(LIBHLA_CLOCK_SRCS
    Clock.cc
    ClockFactory.cc)
list(APPEND LIBHLA_EXPORTED_INCLUDES Clock.hh)

if (HAVE_POSIX_CLOCK)
    list(APPEND LIBHLA_CLOCK_SRCS PosixClock.cc)
    list(APPEND LIBHLA_EXPORTED_INCLUDES PosixClock.hh)
endif(HAVE_POSIX_CLOCK)

if (HAVE_GETTIMEOFDAY)
    list(APPEND LIBHLA_CLOCK_SRCS GettimeofdayClock.cc)
    list(APPEND LIBHLA_EXPORTED_INCLUDES GettimeofdayClock.hh)
endif(HAVE_GETTIMEOFDAY)

if (HAVE_TSC_CLOCK)
    list(APPEND LIBHLA_CLOCK_SRCS TSCClock.cc)
    list(APPEND LIBHLA_EXPORTED_INCLUDES TSCClock.hh)
endif(HAVE_TSC_CLOCK)

if (HAVE_WIN_CLOCK)
   list(APPEND LIBHLA_CLOCK_SRCS WinClock.cc WinPerfClock.cc)
   list(APPEND LIBHLA_EXPORTED_INCLUDES WinClock.hh WinPerfClock.hh)
endif(HAVE_WIN_CLOCK)

source_group("Source Files\\Clock" FILES ${LIBHLA_CLOCK_SRCS})

set(LIBHLA_IPC_SRCS)
list(APPEND LIBHLA_EXPORTED_INCLUDES SHM.hh Semaphore.hh)

if (WIN32)
   list(APPEND LIBHLA_IPC_SRCS SHMWin32.cc SemaphoreWin32.cc)
   list(APPEND LIBHLA_EXPORTED_INCLUDES SHMWin32.hh SemaphoreWin32.hh)
else (WIN32)
   list(APPEND LIBHLA_IPC_SRCS
        SHMPosix.cc SHMSysV.cc
        SemaphorePosix.cc SemaphoreSysV.cc)
   list(APPEND LIBHLA_EXPORTED_INCLUDES
        SHMPosix.hh SHMSysV.hh
        SemaphorePosix.hh SemaphoreSysV.hh)
endif (WIN32)

source_group("Source Files\\IPC" FILES ${LIBHLA_IPC_SRCS})

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# ISO C++ does not support 'long long', but HLAtypesIEEE1516.hh needs it
if(CMAKE_COMPILER_IS_GNUCC)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-long-long")
endif(CMAKE_COMPILER_IS_GNUCC)
if(CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-long-long")
endif(CMAKE_COMPILER_IS_GNUCXX)

add_library(HLA
    ${LIBHLA_TYPES1516_SRCS}
    ${LIBHLA_HASH_SRCS}
    ${LIBHLA_ALLOC_SRCS}
    ${LIBHLA_MB_SRCS}
    ${LIBHLA_CLOCK_SRCS}
    ${LIBHLA_IPC_SRCS}
    ${LIBHLA_EXPORTED_INCLUDES}
)

if (HAVE_POSIX_CLOCK)
    if(RT_LIBRARY)
       target_link_libraries(HLA ${RT_LIBRARY})
    endif(RT_LIBRARY)
endif (HAVE_POSIX_CLOCK)

set_target_properties(HLA PROPERTIES PROJECT_LABEL LibHLA)

set_target_properties(HLA PROPERTIES VERSION ${PACKAGE_VERSION} SOVERSION ${PACKAGE_VERSION_MAJOR})

install(FILES
    ${LIBHLA_EXPORTED_INCLUDES}
    DESTINATION include/libhla)

install(PROGRAMS
    hlaomtdif2cpp.py
    DESTINATION bin)

install(TARGETS HLA
    EXPORT CERTIDepends
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)
