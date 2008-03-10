#!/usr/bin/env python

import os
import getopt, sys
import shutil

def usage():
    print "Usage:\n %s --file=<message> [--verbose] [--help]" % os.path.basename(sys.argv[0])
    
try:
    opts, args = getopt.getopt(sys.argv[1:], "f:vh", ["file=","verbose","help"])
except getopt.GetoptError, err:
    print >> stderr, "opt = %s, msg = %s" % (err.opt,err.msg)
    usage()
    sys.exit(2)


if len(opts) < 1:
    usage()
    sys.exit(2)

# default value
verbose=False

for o, a in opts:
    if o in ("-f", "--file"):
        messagefile=a
    if o in ("-v", "--verbose"):
        verbose=True
    if o in ("-h", "--help"):
        usage()
        sys.exit(0)


msgFile =  open(messagefile,'r')
for l in msgFile:
    cname = l.strip('_ \n')
    cname = cname.title()
    print "class CERTI_EXPORT NM_%s : public NetworkMessage {" % cname
    print "  public:"
    print "  protected:"
    print "  private:"
    print "};\n"

msgFile.close()
