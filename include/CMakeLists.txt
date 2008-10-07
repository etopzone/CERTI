
# Install public HLA 1.3 DLC headers files
INSTALL(FILES 
  NullFederateAmbassador.hh 
  RTI.hh 
  RTIambServices.hh  
  RTItypes.hh 
  baseTypes.hh 
  federateAmbServices.hh 
  fedtime.hh 
  DESTINATION include)
  
# Install public IEEE-1516 DLC headers files
# taken from SISO-STD-004.1-2004
# see http://www.sisostds.org/index.php?tg=fileman&idx=get&id=5&gr=Y&path=SISO+Products%2FSISO+Standards&file=SIS-STD-004.1-2004.zip
INSTALL(DIRECTORY RTI DESTINATION include)
