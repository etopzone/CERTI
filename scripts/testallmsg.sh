#!/bin/sh
rm -f test_messages/*
mkdir -p test_messages
./GenerateMessages.py --input=CERTI_NetworkMessage.msg --language=CXXCERTINetworkMessage --type=header --output=test_messages//NM_Classes.hh
./GenerateMessages.py --input=CERTI_NetworkMessage.msg --language=CXXCERTINetworkMessage --type=body --output=test_messages/NM_Classes.cc
./GenerateMessages.py --input=CERTI_Message.msg --language=CXXCERTIMessage --type=header --output=test_messages/M_Classes.hh
./GenerateMessages.py --input=CERTI_Message.msg --language=CXXCERTIMessage --type=body --output=test_messages/M_Classes.cc
