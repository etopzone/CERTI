# Generate public RTI header with appropriately configured values
# like RTI_VERSION, RTI_INTERNAL_VERSION etc...
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/RTItypes.hh.cmake ${CMAKE_CURRENT_BINARY_DIR}/RTItypes.hh)
# Install public HLA 1.3 DLC headers files
install(FILES
  NullFederateAmbassador.hh
  RTI.hh
  RTIambServices.hh
  ${CMAKE_CURRENT_BINARY_DIR}/RTItypes.hh
  baseTypes.hh
  federateAmbServices.hh
  fedtime.hh
  DESTINATION include/hla13)
