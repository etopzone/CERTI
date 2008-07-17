#!/usr/bin/env python

## ----------------------------------------------------------------------------
## hlaomtdif2cpp.py - HLA OMT DIF document to C++ code convertor
## Copyright (C) 2008  Petr Gotthard <petr.gotthard@centrum.cz>
##
## This library is free software; you can redistribute it and/or
## modify it under the terms of the GNU Lesser General Public
## License version 2.1, as published by the Free Software Foundation.
##
## This library is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
## Lesser General Public License for more details.
##
## $Id: hlaomtdif2cpp.py,v 1.1 2008/07/17 16:03:53 gotthardp Exp $
## ----------------------------------------------------------------------------

import getopt, sys
import re
import xml.sax.handler

# name of the input file
input = None

def usage():
  print "Usage: " + sys.argv[0] + " -i DIF"
  print "Converts HLA OMT DIF document to C++ code."

class CommentFormatter:
  def __init__(self, indent=0, width=80):
    self.lineCount = 0
    self.indent = indent
    self.width = width - indent

  def addComment(self, text):
    stack = [word for word in text.replace("\n", " ").split(" ") if word]
    while stack:
      line = ""
      while stack:
        if len(line) + len(" " + stack[0]) + 3 > self.width: break
        if line: line += " "
        line += stack.pop(0)
      if self.lineCount == 0:
        print " "*self.indent + "/* " + line
      else:
        print " "*self.indent + " * " + line
      self.lineCount += 1

  def addEmptyLine(self):
    print " "*self.indent + " * "

  def finish(self):
    print " "*self.indent + " */"
    self.lineCount = 0

def printComment(text, indent=0, width=80):
  if len(text) == 0 or text == "-NULL-": return

  text = text.replace("\n", " ")
  width = width - indent
  # Single-line comment
  if len(text) + 3 < width:
    print " "*indent + "// " + text
    return
  # Multi-line comment
  formatter = CommentFormatter(indent, width)
  formatter.addComment(text)
  formatter.finish()

class CodeGenerator(xml.sax.handler.ContentHandler):
#  def __init__(self):

  def startElement(self, name, attributes):
    if name == "objectModel":
      print "#line", parser.getLineNumber(), "\"" + input + "\""
      formatter = CommentFormatter()
      formatter.addComment("Name: " + attributes["name"])
      formatter.addComment("Version: " + attributes["version"])
      formatter.addComment("Type: " + attributes["type"])
      formatter.addEmptyLine()
      formatter.addComment("Application domain:")
      formatter.addComment(attributes["appDomain"])
      formatter.addEmptyLine()
      formatter.addComment("Purpose:")
      formatter.addComment(attributes["purpose"])
      formatter.addEmptyLine()
      formatter.addComment("Sponsor:")
      formatter.addComment(attributes["sponsor"])
      formatter.addEmptyLine()
      formatter.addComment("Point of contact:")
      formatter.addComment("Name: " + attributes["pocName"])
      formatter.addComment("Organization: " + attributes["pocOrg"])
      formatter.addComment("Phone: " + attributes["pocPhone"])
      formatter.addComment("E-mail: " + attributes["pocEmail"])
      formatter.finish();
      print

    if name == "simpleData":
      print "#line", parser.getLineNumber(), "\"" + input + "\""
      self.buffer = ""
      printComment(attributes["semantics"])
      print "typedef " + attributes["representation"] + " " + attributes["name"] + ";"

    elif name == "enumeratedData":
      print "#line", parser.getLineNumber(), "\"" + input + "\""
      self.enumCounter = 0
      self.enumRepresentation = attributes["representation"]
      self.enumName = attributes["name"]
      printComment(attributes["semantics"])
      # The enumerations must be isolated from other enumerations in an individual namespace
      print "namespace __" + attributes["name"] + " {"
      print "enum __enum {",

    elif name == "enumerator":
      if self.enumCounter > 0:
        print ",",
      self.enumCounter += 1
      sys.stdout.write("\n  " + attributes["name"] + " = " + attributes["values"])

    elif name == "arrayData":
      print "#line", parser.getLineNumber(), "\"" + input + "\""
      printComment(attributes["semantics"])
      if attributes["encoding"] == "HLAfixedArray":
        print "typedef HLAfixedArray<" + attributes["dataType"] + ", " + attributes["cardinality"] + "> " + attributes["name"] + ";"

      elif attributes["encoding"] == "HLAvariableArray":
        print "typedef HLAvariableArray<" + attributes["dataType"] + "> " + attributes["name"] + ";"

    elif name == "fixedRecordData":
      print "#line", parser.getLineNumber(), "\"" + input + "\""
      printComment(attributes["semantics"])
      # The enumerations must be isolated from other enumerations in an individual namespace
      print "namespace __" + attributes["name"] + " {"
      print "enum __enum {"
      self.recordName = attributes["name"]
      self.recordFields = []

    elif name == "field":
      if len(self.recordFields) > 0:
        sys.stdout.write(",\n")
      printComment(attributes["semantics"], 2)
      sys.stdout.write("  " + attributes["name"])
      if len(self.recordFields) == 0:
        sys.stdout.write(" = 0")
      tupple = (attributes["dataType"], attributes["name"])
      self.recordFields.append(tupple)

#  def characters(self, data):

  def endElement(self, name):
    if name == "simpleDataTypes":
      print "\n",

    if name == "enumeratedData":
      print "\n};"
      print "}"
      print "typedef HLAenumeratedType<__" + self.enumName + "::__enum, " + self.enumRepresentation + "> " + self.enumName + ";\n"

    if name == "arrayDataTypes":
      print "\n",

    elif name == "fixedRecordData":
      print "\n};"
      print "}"
      print "typedef"
      for field in self.recordFields:
        print "  HLAfixedRecord<" + field[0] + ", // " + field[1]
      sys.stdout.write("  HLAfixedRecordEnd")
      for field in self.recordFields:
        sys.stdout.write("> ")
      print self.recordName + ";\n"

try:
  opts, args = getopt.getopt(sys.argv[1:], "i:h", ["help", "output="])
except getopt.GetoptError, err:
  print sys.argv[0] + ": " + str(err) # will print something like "option -a not recognized"
  print "Try `" + sys.argv[0] + " --help' for more information."
  sys.exit(2)
for o, a in opts:
  if o in ("-i", "--input"):
    input = a
    modulename = "__HLAOMTDIF2CPP_" + re.sub('[-._]', '', input.upper()) + "_HH__"
  elif o in ("-h", "--help"):
    usage()
    sys.exit()
  else:
    assert False, "unhandled option"

if input == None:
  print sys.argv[0] + ": missing input file"
  print "Try `" + sys.argv[0] + " --help' for more information."
  sys.exit(2)

print "// Do not edit! This file was automatically generated by",
for arg in sys.argv:
  print arg,
print "\n"
print "#ifndef " + modulename
print "#define " + modulename
print
print "#include <HLAtypesIEEE1516.hh>\n"
print "namespace libhla {\n"

parser = xml.sax.make_parser()
handler = CodeGenerator()
parser.setContentHandler(handler)
parser.parse(input)

print "} // namespace libhla"
print
print "#endif // " + modulename

# $Id: hlaomtdif2cpp.py,v 1.1 2008/07/17 16:03:53 gotthardp Exp $
