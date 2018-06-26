#

all :
	g++ -g -o Interactive_Federate1516 Interactive_Federate1516.cc \
	-L$(CERTI_HOME)/lib -lRTI1516d -lCERTId -lFedTime1516d -DRTI_USES_STD_FSTREAM \
	-I$(CERTI_HOME)/include/ieee1516-2000 -I$(CERTI_HOME)/include/libhla

test_map : test_map.cc
	g++ -g -fpermissive -o test_map test_map.cc \
	-L$(CERTI_HOME)/lib -lRTI1516d -lCERTId -lFedTime1516d -DRTI_USES_STD_FSTREAM \
	-I$(CERTI_HOME)/include/ieee1516-2000 -I$(CERTI_HOME)/include/libhla	





