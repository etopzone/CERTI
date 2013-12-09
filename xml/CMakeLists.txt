# install IEEE provided set of files
# 
install(FILES README.txt DESTINATION share/xml)
install(DIRECTORY ieee1516-2010 DESTINATION share/xml PATTERN "CVS" EXCLUDE)
install(DIRECTORY ieee1516-2000 DESTINATION share/xml PATTERN "CVS" EXCLUDE)