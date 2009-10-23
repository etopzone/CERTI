#!/bin/sh
CERTI_NIGHTLY_BUILD_DIR=`pwd`
/usr/local/bin/ctest -S ${CERTI_NIGHTLY_BUILD_DIR}/certi-ctest.cmake
