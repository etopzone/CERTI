#!/usr/bin/env python

import os
import getopt, sys
import shutil

def usage():
    print "Usage:\n %s --file=<message> [--type=header|body|factory] [--verbose] [--help]" % os.path.basename(sys.argv[0])
    
try:
    opts, args = getopt.getopt(sys.argv[1:], "f:t:vh", ["file=","type=","verbose","help"])
except getopt.GetoptError, err:
    print >> stderr, "opt = %s, msg = %s" % (err.opt,err.msg)
    usage()
    sys.exit(2)


if len(opts) < 1:
    usage()
    sys.exit(2)

# default value
verbose=False
gentype="header"

for o, a in opts:
    if o in ("-f", "--file"):
        messagefile=a
    if o in ("-t", "--type"):
        gentype=a
    if o in ("-v", "--verbose"):
        verbose=True
    if o in ("-h", "--help"):
        usage()
        sys.exit(0)


msgFile =  open(messagefile,'r')
for l in msgFile:
    cname = l.strip('_ \n')    
    if (gentype.lower()=="header"):
        print "/*<BEGIN>---------- %s ------------<BEGIN>*/" % cname.title()
        print "class CERTI_EXPORT NM_%s : public NetworkMessage {" % cname.title()
        print "  public:"
        print "       NM_%s();"  % cname.title()
        print "       virtual ~NM_%s();"  % cname.title()
        print "       virtual void serialize();"
        print "       virtual void deserialize();"
        print "       /* specific Getter/Setter */"
        print "  protected:"
        print "       /* specific field */"
        print "  private:"       
        print "};\n"
        print "/*<END>---------- %s ------------<END>*/\n" % cname.title()
        

    if (gentype.lower()=="body"):
        print "/*<BEGIN>---------- %s ------------<BEGIN>*/" % cname.title()
        print "NM_%s::NM_%s() {"  % (cname.title(),cname.title())
        print "    this->name = \"%s\";" % cname
        print "    this->type = NetworkMessage::%s;" % cname
        print "    /* specific field init */"
        print "}"
        print "NM_%s::~NM_%s() {"  % (cname.title(), cname.title())
        print "}"       
        print "void NM_%s::serialize() {"  % cname.title()
        print "  /* call mother class */      "
        print "  NetworkMessage::serialize(); "
        print "  /* specific code (if any) goes here */"
        print "} /* end of serialize */ "
        print "void NM_%s::deserialize() {" % cname.title()
        print "  /* call mother class */      "
        print "  NetworkMessage::deserialize(); "
        print "  /* specific code (if any) goes here */"
        print "} /* end of deserialize */"
        print "/*<END>---------- %s ------------<END>*/\n" % cname.title()


    if (gentype.lower()=="factory"):
        print "case NetworkMessage::%s:" % cname
        print "   msg = new NM_%s(); " %cname.title()
        print "   break;"

msgFile.close()
