#
# - Define a toplevel package_source_backup target
#   which build a dated tarball
#
# Those CMake macros are provided by the TSP Developer Team
# https://savannah.nongnu.org/projects/tsp
#
IF (UNIX)
  FILE(WRITE ${CMAKE_BINARY_DIR}/PackageBackup.cmake  
"MESSAGE(STATUS \"Package Backup running...\")
INCLUDE(CPackSourceConfig.cmake)
MESSAGE(STATUS \"Package Backup gets username...\")
# get the date
EXECUTE_PROCESS(COMMAND whoami
		TIMEOUT 5
		RESULT_VARIABLE WHO_RES
		OUTPUT_VARIABLE WHO_OUT 
		ERROR_VARIABLE  WHO_ERR
		OUTPUT_STRIP_TRAILING_WHITESPACE)
MESSAGE(STATUS \"Package Backup got \${WHO_OUT}.\")
MESSAGE(STATUS \"Package Backup gets date...\")
# get the date
EXECUTE_PROCESS(COMMAND date +%d%b%Y-%Hh%M
		TIMEOUT 5
		RESULT_VARIABLE DATE_RES
		OUTPUT_VARIABLE DATE_OUT 
		ERROR_VARIABLE  DATE_ERR
		OUTPUT_STRIP_TRAILING_WHITESPACE)
MESSAGE(STATUS \"Package Backup got \${DATE_OUT}.\")
# run cpack
MESSAGE(STATUS \"Package Backup running cpack...\")
EXECUTE_PROCESS(COMMAND cpack -G TGZ --config CPackSourceConfig.cmake
		TIMEOUT 3600
		WORKING_DIRECTORY \${CMAKE_BINARY_DIR})	
MESSAGE(STATUS \"Copy \${CPACK_SOURCE_PACKAGE_FILE_NAME}.tar.gz -->  \${CPACK_SOURCE_PACKAGE_FILE_NAME}-\${WHO_OUT}-\${DATE_OUT}.tar.gz\")
EXECUTE_PROCESS(COMMAND \${CMAKE_COMMAND} -E copy \${CPACK_SOURCE_PACKAGE_FILE_NAME}.tar.gz \${CPACK_SOURCE_PACKAGE_FILE_NAME}-\${WHO_OUT}-\${DATE_OUT}.tar.gz)
MESSAGE(STATUS \"Package backup done in: \${CPACK_SOURCE_PACKAGE_FILE_NAME}-\${WHO_OUT}-\${DATE_OUT}.tar.gz\")"
)
  #
  # Add custom target
  # 
  ADD_CUSTOM_TARGET(package_source_backup
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_BINARY_DIR}/PackageBackup.cmake
    )  

ENDIF(UNIX)
