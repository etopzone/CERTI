# Install public IEEE-1516-2010 DLC headers files
# taken from http://standards.ieee.org/downloads/1516/1516.1-2010/
install(DIRECTORY RTI DESTINATION include/ieee1516-2010 PATTERN "CVS" EXCLUDE)
