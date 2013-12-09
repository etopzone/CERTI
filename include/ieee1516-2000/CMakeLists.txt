# Install public IEEE-1516 DLC headers files
# taken from SISO-STD-004.1-2004
# see http://www.sisostds.org/index.php?tg=fileman&idx=get&id=5&gr=Y&path=SISO+Products%2FSISO+Standards&file=SIS-STD-004.1-2004.zip
install(DIRECTORY RTI DESTINATION include/ieee1516-2000 PATTERN "CVS" EXCLUDE)
