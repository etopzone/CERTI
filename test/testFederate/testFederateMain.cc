#include "testFederate.hh"
#include "testFederate_cmdline.h"

#include <iostream>
#include <sstream>
#include <cstdlib>

/**
 * The CERTI test Federate.
 * This is a sample federate which helps in testing RTI features.
 * This federate comes in 3 flavour:
 *    - CertiTestFederate-HLA13 which uses HLA 1.3 API
 *    - CertiTestFederate-IEEE1516_2000 which uses IEEE-1516 v2000 API
 *    - CertiTestFederate-IEEE1516_2010 which uses IEEE-1516 v2010 API
 */
int main(int argc, char **argv)
{
  gengetopt_args_info args ;
  if (cmdline_parser(argc, argv, &args)) exit(EXIT_FAILURE);
  /* The default verbose level is 2 */
  uint32_t verboseLevel = 2;
  if (args.verbose_given) {
      verboseLevel = args.verbose_arg;
  }

  std::wstring federateName = testFederate::getWString(args.name_arg);
  std::wstring federationName = testFederate::getWString(args.federation_name_arg);
  std::wstring FOMpath = testFederate::getWString(args.fom_model_arg);

  // Create a federate object.
  // This object inherit from appropriate FederateAmbassador class
  // and embbed the appropriate RTIambassador object.
  testFederate myFederate(federateName);
  myFederate.setVerbosityLevel(verboseLevel);
  myFederate.createFederationExecution(federationName,FOMpath);
  myFederate.joinFederationExecution();
  myFederate.getHandles();

  myFederate.resignFederationExecution();
  // Destroy the federation
  myFederate.destroyFederationExecution();

}

