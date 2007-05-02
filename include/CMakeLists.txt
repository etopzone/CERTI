
########### install files ###############

INSTALL(
  FILES 
  NullFederateAmbassador.hh 
  RTI.hh 
  RTIambServices.hh  
  RTItypes.hh 
  baseTypes.hh 
  federateAmbServices.hh 
  fedtime.hh 
  DESTINATION include)

#original Makefile.am contents follow:

## $Id: CMakeLists.txt,v 1.1 2007/05/02 14:50:15 erk Exp $
#
#MAINTAINERCLEANFILES = Makefile.in
#
#noinst_HEADERS = certi.hh
#
#include_HEADERS = NullFederateAmbassador.hh RTI.hh RTIambServices.hh \
#RTItypes.hh baseTypes.hh federateAmbServices.hh fedtime.hh
