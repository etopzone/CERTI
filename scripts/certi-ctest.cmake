####################################################################
# CTest script (new style)
# which may be used to setup a CDash submission 
# to CERTI Dashboard:
#    http://my.cdash.org/index.php?project=CERTI
#
# The script should be tailored to you particular local need
# see the LOCAL SETUP part
#
# Then it should be run like:
#   A) ctest -S certi-ctest.cmake,Experimental
#   B) ctest -S certi-ctest.cmake,Nightly
#
#   A) will run an experimental configure & build & test
#   B) will run a nightly configure & build & test
#
# You may have to add --http1.0 ctest command line option
# if your proxy is refusing HTTP1.1 PUT request
#
# More informations about CTest+CDash usage may be found here
# http://www.cmake.org/Wiki/CMake_Scripting_Of_CTest
# http://techbase.kde.org/Development/CMake/DashboardBuilds
# http://www.cmake.org/Wiki/CTest:Buildserver
####################################################################
cmake_minimum_required(VERSION 2.8)
####################################################################
# BEGINNING OF LOCAL SETUP...
####################################################################
# chose you MY_CTEST_ROOT_DIR
# source checked out and build tree will be put in this directory
set(MY_CTEST_ROOT_DIR "/tmp/CERTI-Test")
if(NOT EXISTS ${MY_CTEST_ROOT_DIR})
  file(MAKE_DIRECTORY ${MY_CTEST_ROOT_DIR})
endif()

set(CTEST_CMAKE_GENERATOR "Unix Makefiles")
set(CTEST_BUILD_COMMAND   "make -j3")
set(CTEST_SITE            "ErkOnTheMove")
set(CTEST_BUILD_NAME      "Linux-x86_64-gcc-4.6.1")
set(CTEST_BUILD_CONFIGURATION "Debug")
# set any extra environment variables to use during 
# the execution of the script here:
set (CTEST_ENVIRONMENT
    #"HTTP_PROXY=<your proxy url here>"
)
####################################################################
# END OF LOCAL SETUP.
####################################################################

set(CTEST_SOURCE_DIRECTORY "${MY_CTEST_ROOT_DIR}/src")
set(CTEST_BINARY_DIRECTORY "${MY_CTEST_ROOT_DIR}/build")
# Empty the binary directory – clean build
ctest_empty_binary_directory("${CTEST_BINARY_DIRECTORY}")
# Write initial cache
#file(WRITE
#    "${CTEST_BINARY_DIRECTORY}/CMakeCache.txt" "
#CMAKE_BUILD_TYPE:String=Debug
#")

set(CTEST_UPDATE_COMMAND "cvs")
set(CTEST_CVS_COMMAND "cvs")
set(CTEST_CVS_CHECKOUT "${CTEST_CVS_COMMAND} -z3 -d:pserver:anonymous@cvs.savannah.nongnu.org:/sources/certi co -d src certi")
#set(CTEST_CVS_UPDATE_OPTIONS )
 
# Easy handling of script command line argument
# CTEST_SCRIPT_ARG is the 'value' that comes after the comma in
# ctest -Syour-ctest-script.cmake,value
set(MODEL Nightly)
if(${CTEST_SCRIPT_ARG} MATCHES Experimental)
  set(MODEL Experimental) 
endif(${CTEST_SCRIPT_ARG} MATCHES Experimental)

set($ENV{LC_MESSAGES}    "en_EN")
# Now start update and configure steps
ctest_start(${MODEL})
ctest_update(SOURCE "${CTEST_SOURCE_DIRECTORY}")
ctest_submit(PARTS Update Notes)
ctest_configure(BUILD "${CTEST_BINARY_DIRECTORY}" SOURCE "${CTEST_SOURCE_DIRECTORY}" APPEND)
ctest_submit(PARTS Configure)
ctest_build(BUILD "${CTEST_BINARY_DIRECTORY}" APPEND)
include("${CTEST_SOURCE_DIRECTORY}/CTestConfig.cmake")
ctest_submit(PARTS Build)
ctest_test(BUILD "${CTEST_BINARY_DIRECTORY}")
ctest_submit(PARTS Test)
