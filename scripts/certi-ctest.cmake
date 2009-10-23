####################################################################
#
# CTest script which may be used to setup a CDash submission
# to CERTI Dashboard.
#
####################################################################
SET (CTEST_SOURCE_DIRECTORY "/LOCAL/ERIC/CertiNightly/CERTI")
SET (CTEST_BINARY_DIRECTORY "/LOCAL/ERIC/CertiNightly/build")

SET (CTEST_CVS_COMMAND "cvs")
SET (CTEST_CVS_CHECKOUT "${CTEST_CVS_COMMAND} -z3 -d:pserver:anonymous@cvs.savannah.nongnu.org:/sources/certi co -d CERTI certi")

# Easy handling of script commande line argument
SET(MODEL Nightly)
IF(${CTEST_SCRIPT_ARG} MATCHES Experimental)
  SET(MODEL Experimental) 
ENDIF(${CTEST_SCRIPT_ARG} MATCHES Experimental)

# which ctest command to use for running the dashboard
IF (${MODEL} MATCHES Experimental)

   SET (CTEST_COMMAND
       "logger -i -t cdash \"CERTI Experimental CDash BEGIN...\" "
       "ctest -D ${MODEL}"
       "logger -i -t cdash \"CERTI Experimental CDash END.\" "
   )
ELSE(${MODEL} MATCHES Experimental)
   SET (CTEST_COMMAND 
     "logger -i -t cdash \"CERTI Nightly CDash BEGIN...\" "
     "ctest -D NightlyStart -D NightlyUpdate -D NightlyConfigure -D NightlyBuild -D NightlyTest -D NightlySubmit"
     "ctest -D NightlyMemCheck -D NightlySubmit"
     "logger -i -t cdash \"CERTI Nightly CDash END.\" "
   )
endif(${MODEL} MATCHES Experimental)

# what cmake command to use for configuring this dashboard
SET (CTEST_CMAKE_COMMAND 
  "cmake"
  )

####################################################################
# The values in this section are optional you can either
# have them or leave them commented out
####################################################################

# should ctest backup the source tree and restore it whenever a test failed?
#SET (CTEST_BACKUP_AND_RESTORE TRUE) 

# should ctest wipe the binary tree before running
#SET (CTEST_START_WITH_EMPTY_BINARY_DIRECTORY TRUE)

# this is the initial cache to use for the binary tree, be careful to escape
# any quotes inside of this string if you use it
SET (CTEST_INITIAL_CACHE "
MAKECOMMAND:STRING=make
CMAKE_MAKE_PROGRAM:FILEPATH=make
CMAKE_GENERATOR:INTERNAL=Unix Makefiles
BUILDNAME:STRING=Linux-i686-gcc-4.1.2
SITE:STRING=ErkAtONERA
CVSCOMMAND:FILEPATH=cvs
")

SET( $ENV{LC_MESSAGES}    "en_EN" )

# set any extra environment variables to use during the execution of the script here:
SET (CTEST_ENVIRONMENT
    "HTTP_PROXY=proxy.onecert.fr:80"
)
