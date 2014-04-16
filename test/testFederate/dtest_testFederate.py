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

def createAnotherOneFederateSequence(federateTester):
    federateTester.timeout = 20
    federateTester.stdout  = file(federateTester.name + ".out",'w+')
    federateTester.stdin   = file(federateTester.name + ".in",'w+')
    federateTester.stderr  = file(federateTester.name + ".err",'w+')
    federateTester.addRunStep("barrier","RTIG started")
    federateTester.addRunStep("runCommand",command=". "+certi_home+"/share/scripts/myCERTI_env.sh "+rtig_param['host'])
    federateTester.addRunStep("runCommand",command="export DISPLAY="+display)
    federateTester.addRunStep("barrier","First Federate started")
    federateTester.addRunStep("runCommand",command=federate_param['path']+" -n \""+federateTester.name+"\"")
#    federateTester.addRunStep("expectFromCommand",pattern="Synchronization")
    federateTester.addRunStep("barrier","All Federate(s) started")
    federateTester.addRunStep("expectFromCommand",pattern="RTIA:: End execution.")
    federateTester.addRunStep("terminateCommand")
    federateTester.addRunStep("barrier","All Federate(s) ended")

try:
    opts, args = getopt.getopt(sys.argv[1:], "hr:b:c:p:d:", ["help","rtig=", "federate=","certi_home=","pseudoexec","display="])
except getopt.GetoptError, err:
    print >> sys.stderr, "opt = %s, msg = %s" % (err.opt,err.msg)
    usage()
    sys.exit(2)

## default values
certi_home_defined=False
display=":0.0"
rtig_param = getUserHostPath("rtig")
federate_param = getUserHostPath("CertiTestFederate-IEEE1516_2000")

for o, a in opts:
    if o in ("--help"):
            usage()
            sys.exit(2)
    if o in ("--pseudoexec"):
        pseudoExecActive=1
    if o in ("-r", "--rtig"):
        rtig_param   = getUserHostPath(a)
    if o in ("-b", "--federate"):
        federate_param = getUserHostPath(a)
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

firstFederate = dtest.DTester("firstFederate",
                        session=dtest.SSHSessionHandler(federate_param['user'],host=federate_param['host']))

rtig = dtest.DTester("rtig",
                     session=dtest.SSHSessionHandler(rtig_param['user'],host=rtig_param['host']))
# you may change the default time out value
rtig.timeout = 40
# you add want to save the output of your dtester to a file.
rtig.stdout    = file(rtig.name + ".out",'w+')
rtig.stdin     = file(rtig.name + ".in",'w+')
rtig.stderr    = file(rtig.name + ".err",'w+')

# describe RTIG run steps
rtig.addRunStep("ok",True,"CERTI RTIG and Federate Starts")
rtig.addRunStep("runCommand",command=". "+certi_home+"/share/scripts/myCERTI_env.sh "+rtig_param['host'])
rtig.addRunStep("runCommand",command=rtig_param['path'])
rtig.addRunStep("expectFromCommand",pattern="CERTI RTIG up and running",timeout=5)
rtig.addRunStep("barrier","RTIG started")
rtig.addRunStep("barrier","All Federate(s) ended")
rtig.addRunStep("terminateCommand")
rtig.addRunStep("waitCommandTermination")
rtig.addRunStep("ok",True,"CERTI RTIG and federate Ends")

# describe federate run steps
firstFederate.timeout = 20
firstFederate.stdout  = file(firstFederate.name + ".out",'w+')
firstFederate.stdin   = file(firstFederate.name + ".in",'w+')
firstFederate.stderr  = file(firstFederate.name + ".err",'w+')
firstFederate.addRunStep("barrier","RTIG started")
firstFederate.addRunStep("runCommand",command=". "+certi_home+"/share/scripts/myCERTI_env.sh "+rtig_param['host'])
firstFederate.addRunStep("runCommand",command="export DISPLAY="+display)
firstFederate.addRunStep("runCommand",command=federate_param['path']+" -n \""+firstFederate.name+"\"")
#firstFederate.addRunStep("expectFromCommand",pattern="Press ENTER to start execution")
firstFederate.addRunStep("barrier","First Federate started")
firstFederate.addRunStep("barrier","All Federate(s) started")
firstFederate.addRunStep("sendToCommand",string="\n")
firstFederate.addRunStep("expectFromCommand",pattern="RTIA:: End execution.")
firstFederate.addRunStep("terminateCommand")
firstFederate.addRunStep("barrier","All Federate(s) ended")

#instanciate more billards
federate2 = dtest.DTester("Federate2",
                        session=dtest.SSHSessionHandler(federate_param['user'],host=federate_param['host']))

federate3 = dtest.DTester("Federate3",
                        session=dtest.SSHSessionHandler(federate_param['user'],host=federate_param['host']))

federate4 = dtest.DTester("Federate4",
                        session=dtest.SSHSessionHandler(federate_param['user'],host=federate_param['host']))

createAnotherOneFederateSequence(federate2)
createAnotherOneFederateSequence(federate3)
createAnotherOneFederateSequence(federate4)

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
    myDTestMaster = dtest.DTestMaster("CERTI test Federate autotest","Launch RTIG + several federate(s)")
    myDTestMaster.registerTraceManager(traceManager)
    myDTestMaster.trace=1

    if (pseudoExecActive):
        myDTestMaster.setPseudoExec(1)
    myDTestMaster.timeout = 40
    myDTestMaster.register(rtig)
    myDTestMaster.register(firstFederate)
    myDTestMaster.register(federate2)
    myDTestMaster.register(federate3)
    #myDTestMaster.register(federate4)
    myDTestMaster.startTestSequence()
    myDTestMaster.waitTestSequenceEnd()

goTest()

