#!/usr/bin/env python

##-----------------------------------------------------------------------
##
## DTest - A Distributed test framework
##
## Copyright (c) 2006-2008 Eric NOULARD, Lionel DUROYON and Frederik DEWEERDT 
##
## This library is free software; you can redistribute it and/or
## modify it under the terms of the GNU Lesser General Public
## License as published by the Free Software Foundation; either
## version 2.1 of the License, or (at your option) any later version.
##
## This library is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
## Lesser General Public License for more details.
##
## You should have received a copy of the GNU Lesser General Public
## License along with this library; if not, write to the Free Software
## Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
##
##-----------------------------------------------------------------------

import logging
import os
import time
import threading
import getopt, sys
import dtest
# uncomment this if you want Promela/SPIN trace
#from dtest.Promela_trace_handler import PromelaTraceHandler

pseudoExecActive=0

def usage():
    print "Usage:\n %s [--help] [--pseudoexec] [--certi_home=<path>] [--display=<display>] --rtig=<user>@[<host>]:<rtig_path> --billard=<user>@[<host>]:<billard_path>" % sys.argv[0]

def getUserHostPath(argument):
    if argument.find("@") != -1:
        (user,argument) = argument.split("@",1)
    else:
        user = os.environ["USER"]
    if argument.find(":") != -1:
        (host,path) = argument.split(":",1)
    else:
        host = "localhost"
        path = argument
    retval = dict()
    retval['user'] = user
    retval['host'] = host
    retval['path'] = path
    return retval

def createAnotherOneBillardSequence(billardTester):
    billardTester.timeout = 20
    billardTester.stdout  = file(billardTester.name + ".out",'w+')
    billardTester.stdin   = file(billardTester.name + ".in",'w+')
    billardTester.stderr  = file(billardTester.name + ".err",'w+')
    billardTester.addRunStep("barrier","RTIG started")
    billardTester.addRunStep("runCommand",command=". "+certi_home+"/share/scripts/myCERTI_env.sh "+rtig_param['host'])
    billardTester.addRunStep("runCommand",command="export DISPLAY="+display)
    billardTester.addRunStep("barrier","First billard started")
##    billardTester.addRunStep("runCommand",command=billard_param['path']+" -n \""+billardTester.name+"\" -t 10 -FTest.fed -fTest")
    billardTester.addRunStep("runCommand",command=billard_param['path']+" -n \""+billardTester.name+"\" -t 10 -FTestxxxx.xml -fTest")
    billardTester.addRunStep("expectFromCommand",pattern="Synchronization")
    billardTester.addRunStep("barrier","All Billard(s) started")
    billardTester.addRunStep("expectFromCommand",pattern="RTIA:: End execution.")
    billardTester.addRunStep("terminateCommand")
    billardTester.addRunStep("barrier","All Billard(s) ended")

try:
    opts, args = getopt.getopt(sys.argv[1:], "hr:b:c:p:d:", ["help","rtig=", "billard=","certi_home=","pseudoexec","display="])
except getopt.GetoptError, err:
    print >> sys.stderr, "opt = %s, msg = %s" % (err.opt,err.msg)
    usage()
    sys.exit(2)

## default values
certi_home_defined=False
display=":0.0"
rtig_param = getUserHostPath("rtig")
billard_param = getUserHostPath("billard")

for o, a in opts:
    if o in ("--help"):
            usage()
            sys.exit(2)
    if o in ("--pseudoexec"):
        pseudoExecActive=1
    if o in ("-r", "--rtig"):
        rtig_param   = getUserHostPath(a)
    if o in ("-b", "--billard"):
        billard_param = getUserHostPath(a)
    if o in ("-c", "--certi_home"):
        certi_home = a
        certi_home_defined=True
    if o in ("--display"):
        display = a;

if not certi_home_defined:
    if os.environ.has_key("CERTI_HOME"):
        certi_home=os.environ["CERTI_HOME"]
    else:
        print "You must define CERTI_HOME environment variable"
        sys.exit(2)

firstBillard = dtest.DTester("firstBillard",
                        session=dtest.SSHSessionHandler(billard_param['user'],host=billard_param['host']))

rtig = dtest.DTester("rtig",
                     session=dtest.SSHSessionHandler(rtig_param['user'],host=rtig_param['host']))
# you may change the default time out value
rtig.timeout = 40
# you add want to save the output of your dtester to a file.
rtig.stdout    = file(rtig.name + ".out",'w+')
rtig.stdin     = file(rtig.name + ".in",'w+')
rtig.stderr    = file(rtig.name + ".err",'w+')

# describe RTIG run steps
rtig.addRunStep("ok",True,"CERTI RTIG and Billard Starts")
rtig.addRunStep("runCommand",command=". "+certi_home+"/share/scripts/myCERTI_env.sh "+rtig_param['host'])
rtig.addRunStep("runCommand",command=rtig_param['path'])
rtig.addRunStep("expectFromCommand",pattern="CERTI RTIG up and running",timeout=5)
rtig.addRunStep("barrier","RTIG started")
rtig.addRunStep("barrier","All Billard(s) ended")
rtig.addRunStep("terminateCommand")
rtig.addRunStep("waitCommandTermination")
rtig.addRunStep("ok",True,"CERTI RTIG and Billard Ends")

# describe billard run steps
firstBillard.timeout = 20
firstBillard.stdout  = file(firstBillard.name + ".out",'w+')
firstBillard.stdin   = file(firstBillard.name + ".in",'w+')
firstBillard.stderr  = file(firstBillard.name + ".err",'w+')
firstBillard.addRunStep("barrier","RTIG started")
firstBillard.addRunStep("runCommand",command=". "+certi_home+"/share/scripts/myCERTI_env.sh "+rtig_param['host'])
firstBillard.addRunStep("runCommand",command="export DISPLAY="+display)
##firstBillard.addRunStep("runCommand",command=billard_param['path']+" -n \""+firstBillard.name+"\" -t 10 -FTest.fed -fTest")
firstBillard.addRunStep("runCommand",command=billard_param['path']+" -n \""+firstBillard.name+"\" -t 10 -FTest.fed -fTest")
firstBillard.addRunStep("expectFromCommand",pattern="Press ENTER to start execution")
firstBillard.addRunStep("barrier","First billard started")
firstBillard.addRunStep("barrier","All Billard(s) started")
firstBillard.addRunStep("sendToCommand",string="\n")
firstBillard.addRunStep("expectFromCommand",pattern="RTIA:: End execution.")
firstBillard.addRunStep("terminateCommand")
firstBillard.addRunStep("barrier","All Billard(s) ended")

#instanciate more billards
billard2 = dtest.DTester("billard2",
                        session=dtest.SSHSessionHandler(billard_param['user'],host=billard_param['host']))

billard3 = dtest.DTester("billard3",
                        session=dtest.SSHSessionHandler(billard_param['user'],host=billard_param['host']))

billard4 = dtest.DTester("billard4",
                        session=dtest.SSHSessionHandler(billard_param['user'],host=billard_param['host']))

createAnotherOneBillardSequence(billard2)
createAnotherOneBillardSequence(billard3)
createAnotherOneBillardSequence(billard4)

# Here begins the test
dtest.DTestMaster.logger.setLevel(level=logging.WARNING)
dtest.DTester.logger.setLevel(level=logging.WARNING)
dtest.SSHSessionHandler.logger.setLevel(level=logging.WARNING)

# Add some trace Handlers
traceManager = dtest.TraceManager()
# TAP goes to stdout
traceManager.register(dtest.TAPTraceHandler())
# MSC goes to file MSC-trace
traceManager.register(dtest.MSCTraceHandler(output="billard-trace.msc"))
# Promela goes to PROMELA_trace
#traceManager.register(PromelaTraceHandler())

def goTest():
    myDTestMaster = dtest.DTestMaster("CERTI Billard autotest","Launch RTIG + several billards")
    myDTestMaster.registerTraceManager(traceManager)
    myDTestMaster.trace=1

    if (pseudoExecActive):
        myDTestMaster.setPseudoExec(1)
    myDTestMaster.timeout = 40
    myDTestMaster.register(rtig)
    myDTestMaster.register(firstBillard)
    myDTestMaster.register(billard2)
    myDTestMaster.register(billard3)
    #myDTestMaster.register(billard4)
    myDTestMaster.startTestSequence()
    myDTestMaster.waitTestSequenceEnd()

goTest()

