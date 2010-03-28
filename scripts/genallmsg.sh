#!/bin/sh
./GenerateMessages.py --input=CERTI_NetworkMessage.msg --language=CXXCERTINetworkMessage --type=header --output=../libCERTI/NM_Classes.hh
./GenerateMessages.py --input=CERTI_NetworkMessage.msg --language=CXXCERTINetworkMessage --type=body --output=../libCERTI/NM_Classes.cc
./GenerateMessages.py --input=CERTI_Message.msg --language=CXXCERTIMessage --type=header --output=../libCERTI/M_Classes.hh
./GenerateMessages.py --input=CERTI_Message.msg --language=CXXCERTIMessage --type=body --output=../libCERTI/M_Classes.cc